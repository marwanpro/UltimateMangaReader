#include "homewidget.h"

#include "ui_homewidget.h"

HomeWidget::HomeWidget(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::HomeWidget),
      currentMangaSource(nullptr),
      filteredMangaTitles(),
      filteredMangaLinks(),
      filteractive(false)
{
    ui->setupUi(this);
    adjustSizes();

    ui->listViewSources->setModel(new QStandardItemModel(this));
    ui->listViewMangas->setModel(new QStringListModel(this));
    activateScroller(ui->listViewSources);
    activateScroller(ui->listViewMangas);

    QObject::connect(ui->lineEditFilter, &CLineEdit::returnPressed, this,
                     &HomeWidget::on_pushButtonFilter_clicked);
}

HomeWidget::~HomeWidget()
{
    delete ui;
}

void HomeWidget::adjustSizes()
{
    ui->pushButtonFilter->setFixedHeight(buttonsize);
    ui->pushButtonFilterClear->setFixedHeight(buttonsize);
    ui->lineEditFilter->setFixedHeight(buttonsize);

    ui->listViewSources->setFixedHeight(listsourcesheight);
    ui->listViewSources->setViewMode(QListView::IconMode);
    ui->listViewSources->setIconSize(QSize(mangasourceiconsize, mangasourceiconsize));
    ui->listViewSources->setStyleSheet("font-size: 8pt");
    ui->listViewMangas->setUniformItemSizes(true);

    ui->listViewMangas->setFocusPolicy(Qt::FocusPolicy::NoFocus);
}

void HomeWidget::updateSourcesList(const QList<AbstractMangaSource *> &sources)
{
    auto model = dynamic_cast<QStandardItemModel *>(ui->listViewSources->model());
    model->clear();

    for (auto ms : sources)
        model->appendRow(listViewItemfromMangaSource(ms));

    refreshMangaListView();
}

QList<QStandardItem *> HomeWidget::listViewItemfromMangaSource(AbstractMangaSource *source)
{
    QList<QStandardItem *> items;
    QStandardItem *item = new QStandardItem(source->name);
    item->setData(QVariant::fromValue(static_cast<void *>(source)));
    item->setIcon(QIcon(QPixmap(":/images/mangahostlogos/" + source->name.toLower() + ".png")));
    item->setSizeHint(QSize(mangasourceitemwidth, mangasourceitemheight));
    item->setText(source->name);
    items.append(item);

    return items;
}

void HomeWidget::on_listViewSources_clicked(const QModelIndex &index)
{
    auto clickedsource = static_cast<AbstractMangaSource *>(index.data(Qt::UserRole + 1).value<void *>());

    emit mangaSourceClicked(clickedsource);
}

void HomeWidget::currentMangaSourceChanged(AbstractMangaSource *source)
{
    currentMangaSource = source;

    refreshMangaListView();
    if (ui->lineEditFilter->text() != "")
        on_pushButtonFilter_clicked();
}

void HomeWidget::on_pushButtonFilter_clicked()
{
    if (currentMangaSource == nullptr || currentMangaSource->mangaList.size == 0)
        return;

    QString ss = ui->lineEditFilter->text();

    filteredMangaLinks.clear();
    filteredMangaTitles.clear();

    if (ss == "")
    {
        filteractive = false;
        refreshMangaListView();
        return;
    }

    for (int i = 0; i < currentMangaSource->mangaList.titles.size(); i++)
        if (currentMangaSource->mangaList.titles[i].contains(ss, Qt::CaseInsensitive))
        {
            filteredMangaTitles.append(currentMangaSource->mangaList.titles[i]);
            filteredMangaLinks.append(currentMangaSource->mangaList.links[i]);
        }

    filteractive = true;
    refreshMangaListView();
}

void HomeWidget::on_pushButtonFilterClear_clicked()
{
    if (ui->lineEditFilter->text() != "")
        ui->lineEditFilter->clear();

    filteredMangaLinks.clear();
    filteredMangaTitles.clear();

    filteractive = false;
    refreshMangaListView();
}

void HomeWidget::refreshMangaListView()
{
    auto model = dynamic_cast<QStringListModel *>(ui->listViewMangas->model());

    if (currentMangaSource == nullptr)
    {
        model->setStringList({});
        return;
    }

    if (!filteractive)
        model->setStringList(currentMangaSource->mangaList.titles);
    else
        model->setStringList(filteredMangaTitles);

    ui->listViewMangas->verticalScrollBar()->setValue(0);
}

void HomeWidget::on_listViewMangas_clicked(const QModelIndex &index)
{
    int idx = index.row();

    QString mangalink = filteractive ? filteredMangaLinks[idx] : currentMangaSource->mangaList.links[idx];

    if (!currentMangaSource->mangaList.absoluteUrls)
        mangalink.prepend(currentMangaSource->baseurl);

    QString mangatitle = filteractive ? filteredMangaTitles[idx] : currentMangaSource->mangaList.titles[idx];

    emit mangaClicked(mangalink, mangatitle);
}
