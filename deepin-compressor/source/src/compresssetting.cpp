#include "compresssetting.h"

#include <QDebug>
#include <DFileDialog>
#include <QFormLayout>


static QStringList compress_typelist = {"zip", "7z", "ar", "cbz", "cpio","exe","iso","tar","tar.7z","tar.Z","tar.bz2","tar.gz","tar.lz","tar.lzma","tar.lzo","tar.xz"};

CompressSetting::CompressSetting(QWidget* parent)
    :QWidget(parent)
{
    m_splitnum = 1;
    InitUI();
    InitConnection();
}

CompressSetting::~CompressSetting()
{

}

void CompressSetting::InitUI()
{
    m_nextbutton = new DPushButton(tr("Compress"));
    m_nextbutton->setFixedWidth(260);


    QWidget* leftwidget = new QWidget();
    m_compressicon = Utils::renderSVG(":/images/Compression Packet.svg", QSize(128, 128));
    m_pixmaplabel = new DLabel();
    m_pixmaplabel->setPixmap(m_compressicon);
    m_compresstype = new QComboBox();
    m_compresstype->setFixedSize(80, 40);
    m_compresstype->addItems(compress_typelist);

    QFormLayout* filelayout = new QFormLayout();
    m_filename = new DLineEdit();
    m_filename->setFixedWidth(230);
    m_filename->setText(tr("untitled file"));
    m_savepath = new DLineEdit();
    m_savepath->setText("~/Desktop");
    m_savepath->setFixedWidth(230);
    m_pathbutton = new Lib_Edit_Button(m_savepath);

    filelayout->addRow(tr("File Name") + ":", m_filename);
    filelayout->addRow(tr("Save to") + ":", m_savepath);
    filelayout->setLabelAlignment(Qt::AlignLeft);
    filelayout->setRowWrapPolicy(QFormLayout::WrapAllRows);

    DLabel* moresetlabel = new DLabel(tr("Advanced Options"));
    m_moresetbutton = new DSwitchButton();
    m_moresetlayout = new QHBoxLayout();
    m_moresetlayout->addWidget(moresetlabel,0 , Qt::AlignLeft);
    m_moresetlayout->addWidget(m_moresetbutton,1 , Qt::AlignRight);


    m_encryptedlabel = new DLabel(tr("Encrypted File")+":");
    m_password = new DPasswordEdit();
    m_encryptedfilelistlabel = new DLabel(tr("Encrypted file list"));
    m_file_secret = new DSwitchButton();
    m_file_secretlayout = new QHBoxLayout();
    m_file_secretlayout->addWidget(m_encryptedfilelistlabel, 0 , Qt::AlignLeft);
    m_file_secretlayout->addWidget(m_file_secret, 1 , Qt::AlignRight);
    m_splitcompress = new DLabel(tr("Separate compression"));
    m_splitlayout = new QHBoxLayout();
    m_splitnumedit = new DLineEdit();
    m_splitnumedit->setValidator(new QIntValidator(D_COMPRESS_SPLIT_MIN, D_COMPRESS_SPLIT_MIX, this));
    m_splitnumedit->setText(QString::number(m_splitnum));
    m_plusbutton = new DPushButton();
    m_plusbutton->setText("+");
    m_plusbutton->setFixedWidth(40);
    m_minusbutton = new DPushButton();
    m_minusbutton->setText("-");
    m_minusbutton->setFixedWidth(40);
    m_splitlayout->addWidget(m_splitnumedit);
    m_splitlayout->addWidget(m_plusbutton);
    m_splitlayout->addWidget(m_minusbutton);
    m_splitlayout->setStretch(0,4);
    m_splitlayout->setStretch(1,1);
    m_splitlayout->setStretch(2,1);


    QVBoxLayout *typeLayout = new QVBoxLayout;
    typeLayout->addStretch();
    typeLayout->addWidget(m_pixmaplabel, 0 , Qt::AlignHCenter | Qt::AlignVCenter);
    typeLayout->addWidget(m_compresstype, 0 , Qt::AlignHCenter | Qt::AlignVCenter);
    typeLayout->addStretch();
    leftwidget->setLayout(typeLayout);

    m_fileLayout = new QVBoxLayout;
    m_fileLayout->addStretch();
    m_fileLayout->addLayout(filelayout);
    m_fileLayout->addStretch();
    m_fileLayout->addLayout(m_moresetlayout);

    QHBoxLayout *infoLayout = new QHBoxLayout();
    infoLayout->addStretch();
    infoLayout->addWidget(leftwidget);
    infoLayout->addStretch();
    infoLayout->addLayout(m_fileLayout);
    infoLayout->addStretch();




    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(20);
    mainLayout->addStretch();
    mainLayout->addLayout(infoLayout);
    mainLayout->addStretch();
    mainLayout->addWidget(m_nextbutton, 0 , Qt::AlignHCenter | Qt::AlignVCenter);


}
void CompressSetting::InitConnection()
{
    connect(m_pathbutton, &DPushButton::clicked, this, &CompressSetting::onPathButoonClicked);
    connect(m_nextbutton, &DPushButton::clicked, this, &CompressSetting::onNextButoonClicked);
    connect(m_moresetbutton, &DSwitchButton::toggled, this, &CompressSetting::onAdvanceButtonClicked);
    connect(m_plusbutton, &DPushButton::clicked, this, &CompressSetting::onPlusButoonClicked);
    connect(m_minusbutton, &DPushButton::clicked, this, &CompressSetting::onLessButoonClicked);
}


void CompressSetting::onPathButoonClicked()
{
    DFileDialog dialog;
    dialog.setAcceptMode(DFileDialog::AcceptOpen);
    dialog.setFileMode(DFileDialog::Directory);
    dialog.setDirectory("~/Desktop");

    const int mode = dialog.exec();

    if (mode != QDialog::Accepted) {
        return;
    }

    QList<QUrl> pathlist = dialog.selectedUrls();

    QString curpath = pathlist.at(0).toLocalFile();
    m_savepath->setText(curpath);

}

void CompressSetting::onNextButoonClicked()
{
    QMap<QString, QString> m_openArgs;
    const QUrl saveFileUrl = m_savepath->text() + "/" + m_filename->text() + "." + m_compresstype->currentText();
    const QString password = m_password->text();
    const QString fixedMimeType = m_compresstype->currentText();


    m_openArgs[QStringLiteral("createNewArchive")] = QStringLiteral("true");
    m_openArgs[QStringLiteral("fixedMimeType")] = fixedMimeType;
    m_openArgs[QStringLiteral("compressionLevel")] = 5;//5 is default

    if (m_splitnum > 0) {
        m_openArgs[QStringLiteral("volumeSize")] = QString::number(m_splitnum);
    }
//    if (!dialog.data()->compressionMethod().isEmpty()) {
//        m_openArgs.metaData()[QStringLiteral("compressionMethod")] = dialog.data()->compressionMethod();
//    }
    if (!m_password->text().isEmpty()) {
        m_openArgs[QStringLiteral("encryptionMethod")] = "AES256";//5 is default
    }

    m_openArgs[QStringLiteral("encryptionPassword")] = password;

    if (m_file_secret->isChecked()) {
        m_openArgs[QStringLiteral("encryptHeader")] = QStringLiteral("true");
    }

//    openUrl(saveFileUrl);

    emit sigCompressPressed(m_openArgs);

    m_openArgs.remove(QStringLiteral("showExtractDialog"));
    m_openArgs.remove(QStringLiteral("createNewArchive"));
    m_openArgs.remove(QStringLiteral("fixedMimeType"));
    m_openArgs.remove(QStringLiteral("compressionLevel"));
    m_openArgs.remove(QStringLiteral("encryptionPassword"));
    m_openArgs.remove(QStringLiteral("encryptHeader"));
}

void CompressSetting::onAdvanceButtonClicked(bool status)
{
    if(status)
    {
        m_encryptedlabel->setVisible(true);
        m_password->setVisible(true);
        m_splitcompress->setVisible(true);
        m_encryptedfilelistlabel->setVisible(true);
        m_file_secret->setVisible(true);
        m_splitcompress->setVisible(true);
        m_splitnumedit->setVisible(true);
        m_plusbutton->setVisible(true);
        m_minusbutton->setVisible(true);
        m_fileLayout->addWidget(m_encryptedlabel);
        m_fileLayout->addWidget(m_password);
        m_fileLayout->addLayout(m_file_secretlayout);
        m_fileLayout->addWidget(m_splitcompress);
        m_fileLayout->addLayout(m_splitlayout);
    }
    else {

        m_fileLayout->removeWidget(m_encryptedlabel);
        m_fileLayout->removeWidget(m_password);
        m_fileLayout->removeItem(m_file_secretlayout);
        m_fileLayout->removeWidget(m_splitcompress);
        m_fileLayout->removeItem(m_splitlayout);
        m_encryptedlabel->setVisible(false);
        m_password->setVisible(false);
        m_splitcompress->setVisible(false);
        m_encryptedfilelistlabel->setVisible(false);
        m_file_secret->setVisible(false);
        m_splitcompress->setVisible(false);
        m_splitnumedit->setVisible(false);
        m_plusbutton->setVisible(false);
        m_minusbutton->setVisible(false);
    }
}


void CompressSetting::onPlusButoonClicked()
{
    if(m_splitnum < D_COMPRESS_SPLIT_MIX)
    {
        m_splitnum ++;
        m_splitnumedit->setText(QString::number(m_splitnum));
    }
}

void CompressSetting::onLessButoonClicked()
{
    if(m_splitnum > D_COMPRESS_SPLIT_MIN)
    {
        m_splitnum --;
        m_splitnumedit->setText(QString::number(m_splitnum));
    }
}

void CompressSetting::onRetrunPressed()
{

}
