#include "moodform.h"
#include "../utils/bae.h"
#include "../db/collectionDB.h"

#include <QToolButton>
#include <QGridLayout>
#include <QButtonGroup>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QFrame>
#include <QPushButton>

MoodForm::MoodForm(QWidget *parent) :
    QWidget(parent)
{
    this->setWindowTitle("Moods");
    this->setWindowFlags(Qt::Dialog);
    this->setWindowModality(Qt::ApplicationModal);

    auto moodMap = BAE::loadSettings("MOODS", "SETTINGS", QMap<QString, QVariant>()).toMap();

    auto moodsLayout = new QGridLayout;

    QList<QLineEdit*> edits;

    auto moods = BAE::MoodColors;

    for(int i=0; i<moods.size(); i++)
    {
        auto colorTag = new QToolButton(this);
        colorTag->setAutoRaise(true);
        colorTag->setStyleSheet(QString("QToolButton { background-color: %1;}").arg(moods.at(i)));
        colorTag->setEnabled(false);

        auto lineEdit = new QLineEdit(this);
        lineEdit->setFrame(false);
        lineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        lineEdit->setText(moodMap[moods[i]].toStringList().join(","));
        edits << lineEdit;

        auto layout = new QHBoxLayout;
        auto frame = new QFrame(this);
        frame->setMaximumHeight(colorTag->size().height());
        frame->setFrameStyle(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Sunken);
        frame->setLayout(layout);
        layout->setContentsMargins(0,0,0,0);
        layout->setSpacing(0);

        layout->addWidget(colorTag);
        layout->addWidget(lineEdit);

        moodsLayout->addWidget(frame,i,0);
    }
    auto acceptBtn = new QPushButton("Save", this);
    connect(acceptBtn, &QPushButton::clicked,[=]()
    {
        QMap<QString, QVariant> map;
        auto i = 0;
        for(auto line: edits)
        {
            map.insert(moods[i], line->text().trimmed().split(","));
            i++;
        }

        BAE::saveSettings("MOODS", map, "SETTINGS");
        qDebug()<<map;
       emit this->saved();
    });

    moodsLayout->addWidget(acceptBtn,moods.size(),0);
    this->setLayout(moodsLayout);
}

MoodForm::~MoodForm()
{}
