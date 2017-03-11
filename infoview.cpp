#include "infoview.h"
#include "ui_infoview.h"


InfoView::InfoView(QWidget *parent) : QWidget(parent), ui(new Ui::InfoView) {
    ui->setupUi(this);
    artist = new Album(":Data/data/cover.png", 120, 100,false,false,this);
    connect(artist,SIGNAL(playAlbum(QString , QString)),this,SLOT(playAlbum_clicked(QString, QString)));

    ui->lyricsText->setLineWrapMode(QTextEdit::NoWrap);
    ui->lyricsText->setStyleSheet(
                "QTextBrowser{background-color: #575757; color:white;}");

    lyrics = new Lyrics();
    connect(lyrics,SIGNAL(lyricsReady(QString)),this,SLOT(setLyrics(QString)));


    artist->titleVisible(false);
    artist->borderColor = true;
    auto artistContainer = new QWidget();
    artistContainer->setBackgroundRole(QPalette::Dark);
    auto artistCLayout = new QHBoxLayout();
    auto *left_spacer = new QWidget();
    left_spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto *right_spacer = new QWidget();
    right_spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    artistCLayout->addWidget(left_spacer);
    artistCLayout->addWidget(artist);
    artistCLayout->addWidget(right_spacer);
    artistContainer->setLayout(artistCLayout);
    artistContainer->setMaximumSize(200, 200);
    ui->artistLayout->insertWidget(0, artistContainer);

    infoUtils = new QWidget();
    // artistContainer->setStyleSheet("QWidget{background-color: #575757;
    // color:white;}");
    infoUtils->setMaximumWidth(200);
    auto infoUtils_layout = new QHBoxLayout();
    infoUtils_layout->setContentsMargins(0, 0, 0, 0);
    infoUtils_layout->setSpacing(0);
    auto similarBtn = new QToolButton();
    similarBtn->setAutoRaise(true);
    similarBtn->setIcon(QIcon::fromTheme("similarartists-amarok"));

    auto moreBtn = new QToolButton();
    moreBtn->setAutoRaise(true);
    moreBtn->setIcon(QIcon::fromTheme("filename-discnumber-amarok"));


    hideBtn = new QToolButton();
    connect(hideBtn, SIGNAL(clicked()), this, SLOT(hideArtistInfo()));

    hideBtn->setAutoRaise(true);
    hideBtn->setIcon(QIcon::fromTheme("hide_table_column"));

    infoUtils_layout->addWidget(similarBtn);
    infoUtils_layout->addWidget(moreBtn);

    infoUtils_layout->addWidget(hideBtn);
    infoUtils_layout->addWidget(ui->searchBtn);
    infoUtils->setLayout(infoUtils_layout);
    ui->artistFrame->setVisible(false);
    ui->frame_4->setVisible(false);
    infoUtils->hide();
    ui->customsearch->setVisible(false);
    ui->frame_2->setVisible(false);
    //ui->searchBtn->setParent(ui->lyricsText);
    // ui->searchBtn->setGeometry(5,5,22,22);

    //ui->searchBtn->setVisible(false);

}

InfoView::~InfoView() { delete ui; }


void InfoView::playAlbum_clicked(QString artist, QString album)
{
    emit playAlbum(artist,album);

}

void InfoView::hideArtistInfo() {
    qDebug() << "hide artist info";
    if (hide) {
        ui->artistFrame->setVisible(false);
        ui->frame_4->setVisible(false);
        hideBtn->setIcon(QIcon::fromTheme("show_table_column"));
        hide = !hide;
    } else {
        ui->artistFrame->setVisible(true);
        ui->frame_4->setVisible(true);
        hideBtn->setIcon(QIcon::fromTheme("hide_table_column"));
        hide = !hide;
    }
}

void InfoView::setAlbumInfo(QString info) {

    //qDebug() << info;
    if (info.isEmpty()) {
        ui->albumText->hide();

        ui->frame_5->hide();
    } else {
        ui->albumText->show();

        ui->frame_5->show();
        ui->albumText->setHtml(info);
    }
}

void InfoView::setAlbumArt(QByteArray array) {Q_UNUSED(array)}

void InfoView::setArtistInfo(QString info) { ui->artistText->setHtml(info); }

void InfoView::setArtistArt(QByteArray array) { artist->putPixmap(array); }

void InfoView::setLyrics(QString lyrics) {
    ui->lyricsText->setHtml(lyrics);
    ui->lyricsLayout->setAlignment(Qt::AlignCenter);
    ui->lyricsFrame->show();
}

void InfoView::on_searchBtn_clicked()
{
    if(!customsearch)
    {
        ui->customsearch->setVisible(true);
        ui->frame_2->setVisible(true);
        customsearch=!customsearch;
    }
    else
    {
        ui->customsearch->setVisible(false);
        ui->frame_2->setVisible(false);
        customsearch=!customsearch;
    }
}


void InfoView::getTrackInfo(QString title, QString artist, QString album)
{
    if(!album.isEmpty()&&!artist.isEmpty())
    {
        ArtWork coverInfo;
        ArtWork artistInfo;
        connect(&coverInfo, SIGNAL(infoReady(QString)), this, SLOT(setAlbumInfo(QString)));
        connect(&artistInfo, SIGNAL(bioReady(QString)), this, SLOT(setArtistInfo(QString)));
        coverInfo.setDataCoverInfo(artist,album);
        artistInfo.setDataHeadInfo(artist);

        if(!title.isEmpty())
        {
            lyrics->setData(artist,title);
        }
    }
}



void InfoView::on_toolButton_clicked()
{
    QString artist=ui->artistLine->text();
    QString title=ui->titleLine->text();

    if(!artist.isEmpty()&&!title.isEmpty())
    {

        lyrics->setData(artist,title);

    }
}
