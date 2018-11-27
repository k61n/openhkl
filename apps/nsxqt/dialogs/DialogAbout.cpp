#include <vector>

#include <QBoxLayout>
#include <QDate>
#include "QDesktopServices"
#include <QFont>
#include <QLabel>
#include <QPixmap>
#include <QTextEdit>
#include <QPushButton>
#include "QUrl"
#include <QTextBrowser>
#include <QVBoxLayout>

#include <nsxlib/Version.h>

#include "CustomEventFilters.h"
#include "DialogAbout.h"

#include <QDebug>

namespace
{

QLabel* createTitle()
{
    QString title_string("NSXTool (version ");
    title_string.append(nsx::__version__.c_str());
    title_string.append(" , commit ");
    auto commit = nsx::__commit_id__.c_str();
    title_string.append(QString("<a href=\"https://code.ill.fr/scientific-software/nsxtool/commit/%1\">%2</a>)").arg(commit).arg(commit));

    QLabel* title = new QLabel();

    title->setTextFormat(Qt::RichText);
    title->setTextInteractionFlags(Qt::TextBrowserInteraction);
    title->setOpenExternalLinks(true);
    title->setStyleSheet("a{text-decoration: none;}");

    title->setText(title_string);

    title->setAlignment(Qt::AlignHCenter);

    return title;
}

QLabel* createSubTitle()
{
    QLabel* subtitle = new  QLabel("A software for reducing single crystal diffraction data.");

    subtitle->setAlignment(Qt::AlignHCenter);

    return subtitle;
}

QTextBrowser* createAboutText()
{
    auto about = new QTextBrowser();

    about->document()->setDefaultStyleSheet("a{text-decoration: none;}");

    about->append("Created by:");
    about->append("    Laurent Chapon (ILL)");
    about->append("    Eric Pellegrini (ILL)");

    about->append("");

    about->append("Current developer(s):");
    about->append("    Eric Pellegrini (ILL)");

    about->append("");

    about->append("Formers developer(s):");
    about->append("    Laurent Chapon (ILL)");
    about->append("    Jonathan Fisher (FRM2/JCNS)");

    about->append("");

    about->append("Copyright: Institut Laue Langevin");

    return about;
}

QLabel* createLogoLabel(int width)
{
    QPixmap logo(":/resources/aboutNSXTool", "JPG");
    auto result = new QLabel;
    auto scaled_logo = logo.scaled(width-20, 200, Qt::IgnoreAspectRatio);
    result->setPixmap(scaled_logo);
    result->setFixedSize(scaled_logo.rect().size());
    result->setAlignment(Qt::AlignHCenter);
    return result;
}

}

DialogAbout::DialogAbout(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("About NSXTool");
    setWindowFlags(Qt::Dialog);

    setFixedSize(800,500);

    QFont titleFont;
    titleFont.setPointSize(16);
    titleFont.setBold(true);

    QFont italicFont;
    italicFont.setPointSize(14);
    italicFont.setBold(false);
    italicFont.setStyle(QFont::StyleItalic);

    // title
    auto title_label = createTitle();
    title_label->setFont(titleFont);
    title_label->setContentsMargins(0, 0, 0, 15);

    auto subtitle_label = createSubTitle();
    subtitle_label->setFont(italicFont);
    subtitle_label->setContentsMargins(0, 0, 0, 15);
    subtitle_label->setWordWrap(true);

    QHBoxLayout* title_layout = new QHBoxLayout();
    QVBoxLayout* title_text_layout = new QVBoxLayout();

    title_layout->addWidget(createLogo());
    title_text_layout->addWidget(title_label);
    title_text_layout->addWidget(subtitle_label);
    title_layout->addLayout(title_text_layout);

    QHBoxLayout* about_layout = new QHBoxLayout();
    about_layout->addWidget(createAboutText());

    QVBoxLayout* main_layout = new QVBoxLayout();
    main_layout->addLayout(title_layout);
    main_layout->addLayout(about_layout);
    main_layout->addLayout(createPartnersLayout());

    setLayout(main_layout);

    static const char mydata[] = {0x45,0x75,0x72,0x79,0x64,0x69,0x63,0x65};
    QByteArray b = QByteArray::fromRawData(mydata, sizeof(mydata));
    auto f = new ShortcodeFilter(b, this);
    connect(f, &ShortcodeFilter::found, this, [=]() {
        main_layout->addWidget(createLogoLabel(size().width()));
        main_layout->update();
    });
    installEventFilter(f);
}

QPushButton* DialogAbout::createLogo()
{
    auto nsxtool_logo = new QPushButton;

    nsxtool_logo->setStyleSheet("QPushButton::focus{border: none; outline: none; background: none}");

    QPixmap nsxtool_logo_pixmap(":/resources/crystalIcon", "PNG");
    auto nsxtool_logo_pixmap_scaled = nsxtool_logo_pixmap.scaled(80,80, Qt::KeepAspectRatio);
    nsxtool_logo->setIcon(QIcon(nsxtool_logo_pixmap_scaled));
    nsxtool_logo->setIconSize(nsxtool_logo_pixmap_scaled.rect().size());
    nsxtool_logo->setFixedSize(nsxtool_logo_pixmap_scaled.rect().size());
    connect(nsxtool_logo,&QPushButton::clicked,[](){QDesktopServices::openUrl(QUrl("https://code.ill.fr/scientific-software/nsxtool.git"));});

    return nsxtool_logo;
}

QHBoxLayout* DialogAbout::createPartnersLayout()
{
    QHBoxLayout* layout = new QHBoxLayout();

    QPixmap ill_logo_pixmap(":/resources/ill_logo.png", "PNG");
    QPushButton *ill_logo = new QPushButton();
    ill_logo->setStyleSheet("QPushButton::focus{background: none}");
    auto ill_logo_pixmap_scaled = ill_logo_pixmap.scaled(100,100, Qt::KeepAspectRatio);
    ill_logo->setIcon(QIcon(ill_logo_pixmap_scaled));
    ill_logo->setIconSize(ill_logo_pixmap_scaled.rect().size());
    ill_logo->setFixedSize(ill_logo_pixmap_scaled.rect().size());
    connect(ill_logo,&QPushButton::clicked,[](){QDesktopServices::openUrl(QUrl("https://www.ill.eu/en"));});

    QPixmap jcns_logo_pixmap(":/resources/jcns_logo.png", "PNG");
    QPushButton *jcns_logo = new QPushButton();
    jcns_logo->setStyleSheet("QPushButton::focus{background: none}");
    auto jcns_logo_pixmap_scaled = jcns_logo_pixmap.scaled(200,200, Qt::KeepAspectRatio);
    jcns_logo->setIcon(QIcon(jcns_logo_pixmap_scaled));
    jcns_logo->setIconSize(jcns_logo_pixmap_scaled.rect().size());
    jcns_logo->setFixedSize(jcns_logo_pixmap_scaled.rect().size());
    connect(jcns_logo,&QPushButton::clicked,[](){QDesktopServices::openUrl(QUrl("http://www.fz-juelich.de/jcns/EN/Home/home_node.html"));});

    QPixmap frm2_logo_pixmap(":/resources/frm2_logo.png", "PNG");
    QPushButton *frm2_logo = new QPushButton();
    frm2_logo->setStyleSheet("QPushButton::focus{background: none}");
    auto frm2_logo_pixmap_scaled = frm2_logo_pixmap.scaled(120,120, Qt::KeepAspectRatio);
    frm2_logo->setIcon(QIcon(frm2_logo_pixmap_scaled));
    frm2_logo->setIconSize(frm2_logo_pixmap_scaled.rect().size());
    frm2_logo->setFixedSize(frm2_logo_pixmap_scaled.rect().size());
    connect(frm2_logo,&QPushButton::clicked,[](){QDesktopServices::openUrl(QUrl("http://www.frm2.tum.de/en/home/"));});

    layout->addWidget(ill_logo);
    layout->addWidget(jcns_logo);
    layout->addWidget(frm2_logo);

    return layout;
}
