/*
 * Written by SaEeD
 *
 */

#include "dialog.h"
#include "ui_dialog.h"
#include "cicontoimage.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    ui->lblIcon->addAction(ui->actionSave_to_File);
    ui->txtIconFile->addAction(ui->actionChoose_Icon_File);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_actionSave_to_File_triggered()
{
    qDebug() << "Saving to file!";

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                 ".",
                                 tr("Icon (*.ico *.png)"));
    if(fileName.isEmpty())
    {
        QMessageBox::warning(this, tr("Error"),
                                         tr("Cannot Save the File!"),
                                         QMessageBox::Ok | QMessageBox::Cancel);
        return;
    }
    ui->lblIcon->pixmap()->toImage().save(fileName);

}

void Dialog::on_actionChoose_Icon_File_triggered()
{
    QString path = QFileDialog::getOpenFileName(this,
                                                tr("Open Image"), ".", tr("Image Files (*.ico)"));
    if(path.isEmpty())
    {
        QMessageBox::warning(this, tr("Error"),
                                         tr("Nothing has been selected!"),
                                         QMessageBox::Ok | QMessageBox::Cancel);
        ui->txtIconFile->setText(path);
        return;
    }
    ui->txtIconFile->setText(path);
    ui->lblIcon->setPixmap(QPixmap(path));
}

void Dialog::on_txtPEFile_textChanged(const QString &arg1)
{
    //qDebug() << QFileInfo(arg1).exists();

    if(!arg1.isEmpty() && QFileInfo(arg1).exists() )
    {
        ui->btnUpdateIcon->setEnabled(true);
    }else{
        ui->btnUpdateIcon->setEnabled(false);
    }
}


//Loading PE file and its icon
void Dialog::on_btnLoad_clicked()
{
    QString path = QFileDialog::getOpenFileName(this,
                                                tr("Open Executable"), ".", tr("PE File (*.exe *.dll)"));
    if(path.isEmpty())
    {
        QMessageBox::warning(this, tr("Error"),
                                         tr("Nothing has been selected!"),
                                         QMessageBox::Ok | QMessageBox::Cancel);
        return;
    }
    ui->txtPEFile->setText(path);

    HMODULE hLibrary;
    HRSRC hResource;
    HGLOBAL hResourceLoaded;
    LPBYTE lpBuffer;
    DWORD sizeIcon;


    std::wstring stemp = ui->txtPEFile->text().toStdWString();
    LPCWSTR FileName = stemp.c_str();

    //https://msdn.microsoft.com/en-us/library/windows/desktop/ms684179(v=vs.85).aspx
    // refere to LOAD_LIBRARY_AS_DATAFILE section

    hLibrary = LoadLibraryEx(FileName,NULL, LOAD_LIBRARY_AS_DATAFILE); //
    if (NULL != hLibrary)
    {
        qDebug() << "Loading PE file..." ;

        int l;
        for (l=1;l<100;l++){
        hResource = FindResource(hLibrary, MAKEINTRESOURCE(l), RT_ICON);
        if(hResource!=NULL){

            //the size here helps to load Icon resourced with low byte quality
            //recommended for old files
                     sizeIcon = SizeofResource(hLibrary, hResource);
                     if ((sizeIcon < 5000)&&(sizeIcon>700))
                     {
                         qDebug() << l;
                         break;
                     }
            }
        }

        hResource = FindResource(hLibrary,MAKEINTRESOURCE(l),RT_ICON);
        qDebug() << "Resource : " << hResource;


        if (NULL != hResource)
        {
            qDebug() << "Searching for ICON resource...";
            hResourceLoaded = LoadResource(hLibrary, hResource);
            if (NULL != hResourceLoaded)
            {
                //Obtain the ICON resource in RAW format, ICON file header needs to be added
                lpBuffer = (LPBYTE) LockResource(hResourceLoaded);
                if (NULL != lpBuffer)
                {
                    // do something with lpBuffer here

                    qDebug() << "Resource loaded into buffer.\nExtracting .ICO file.";

                    //ICON file header is the HEX below and the order is like this:
                    // header + ICON size (Big endian order) + header2

                    unsigned char header[] = "\x00\x00\x01\x00\x01\x00\x20\x40\x00\x00\x01\x00\x04";
                    unsigned char header2[] = "\x00\x00\x16\x00\x00";

                    QByteArray buf;
                    buf.append(QByteArray::QByteArray((const char*) header, sizeof(header)));

                    //little endian to big-endian byte order for ICON file size
                    buf.append((int)sizeIcon & 0xFF );
                    buf.append( ((int)sizeIcon & 0xFF00) >>8 );

                    buf.append(QByteArray::QByteArray((const char*) header2, sizeof(header2)));
                    buf.append(QByteArray::QByteArray((const char*) lpBuffer, (int)sizeIcon));

                    //qDebug() << buf;
                    qDebug() <<"[+]ICON file size: "  << buf.length() << " Bytes";

                    QString tempfile = ui->txtPEFile->text().mid(0,ui->txtPEFile->text().lastIndexOf('.'));
                    tempfile = tempfile.mid(ui->txtPEFile->text().lastIndexOf('/')+1);
                   // qDebug() << tempfile;

                    //Saving the ICON file to current folder for further use
                    QFile file("./" + tempfile + ".ico");
                    file.open(QIODevice::WriteOnly);
                    file.write(buf);
                    file.close();

                    QFileInfo fi(file);
                    qDebug() << fi.absoluteFilePath();

                    load(fi.absoluteFilePath().toStdWString().c_str() ,fi.absoluteFilePath().toStdWString().c_str() );
                    QPixmap pixmap(fi.absoluteFilePath()+ ".png");

                    ui->lblIcon->setPixmap(pixmap);

                }
            }
        }else{
            qDebug() << "[!]Cannot Locar Resource." ;
            ui->lblIcon->setText("[!]Cannot load ICON resource.");

        }

        FreeLibrary(hLibrary);
    }else{
        qDebug() << "Error Loading PE file!!!" ;
    }

}

//Update PE Icon file, there are still some bugs and it does not work on all PE files,
//however, it works on most
void Dialog::on_btnUpdateIcon_clicked()
{
    qDebug() << "[+]Updating ICON Resource.";
    if(!QFileInfo(ui->txtIconFile->text()).exists())
    {
        QMessageBox::warning(this, tr("Error"),
                                         tr("Cannot find the ICON file."),
                                         QMessageBox::Ok | QMessageBox::Cancel);
        return;
    }

    auto MsgBoxRet = QMessageBox::warning(this, tr("Warning!"),
                                     tr("WARNING!!! All the changes will overwrite the file and CANNOT be undone.\nAre you Sure?"),
                                     QMessageBox::Yes | QMessageBox::No);
    if(MsgBoxRet == QMessageBox::No)
    {
        qDebug() << "[!]Canceled by user!";
        return;
    }


    char *buffer;    // buffer to store raw icon data
    streamsize buffersize; // length of buffer
    ifstream hFile;       // file handle

    hFile.open(ui->txtIconFile->text().toStdWString(), ios::in| ios::binary | ios::ate);
    if (!hFile.is_open())
    {
        QMessageBox::warning(this, tr("Error"),
                                         tr("Cannot open the ICON file."),
                                         QMessageBox::Ok | QMessageBox::Cancel);
        return;
    }

    //Reading the file size by moving the file pointer to last byte and then back to first
    buffersize = hFile.tellg();
    hFile.seekg(0, std::ios::beg);
    qDebug()  << "[+]File size: " << buffersize;

    buffer = (char *)malloc(buffersize);

    //Filling the buffer with file's contents
    if(!hFile.read(buffer, buffersize))
    {
        qDebug() << "[!]Error reading file.";
    }
    qDebug() << "[+]File content copied to buffer.";

    //refer to MSDN for more details

    HANDLE hRes = BeginUpdateResource(ui->txtPEFile->text().toStdWString().c_str(), FALSE);


    //First creating ICON GROUP with the necessary headers and add them to PE file
    char temp[16 + 4];
    for(int x=0; x<17; x++)
    {
        temp[x] = buffer[x];
    }
    memcpy(&temp[16],"\x00\x00\x00\x00" ,4);
    temp[18] = '\x01';


    UpdateResource(
         hRes,  // Handle to executable
         RT_GROUP_ICON, // Resource type - icon
         MAKEINTRESOURCE(1), // Make the id 1
         MAKELANGID(LANG_ENGLISH,
                    SUBLANG_DEFAULT), // Default language
         temp,
         // skip the first 22 bytes because this is the
         // icon header&directory entry (if the file
         // contains multiple images the directory entries
         // will be larger than 22 bytes
         20  // length of buffer
        );



    //5 here is just a random number i used to replace the first 5 ICONs if they exist
    for(int i=1; i<5; i++)
    {
        UpdateResource(
             hRes,  // Handle to executable
             RT_ICON, // Resource type - icon
             MAKEINTRESOURCE(i), // Make the id 1
             MAKELANGID(LANG_ENGLISH,
                        SUBLANG_DEFAULT), // Default language
             (buffer+22),
             // skip the first 22 bytes because this is the
             // icon header&directory entry (if the file
             // contains multiple images the directory entries
             // will be larger than 22 bytes
             buffersize-22  // length of buffer
            );

        EndUpdateResource(hRes, FALSE);
    }


    hFile.close();
    delete buffer;
    EndUpdateResource(hRes, FALSE);

}
