#include "mainwidget.h"

#include <QDebug>
#include <QCoreApplication>
#include <QAction>
#include <QBoxLayout>
#include <QFileSystemModel>
#include <QDesktopServices>
#include <QStandardItemModel>

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
{
    doLayout();
}

MainWidget::~MainWidget()
{
    
}

void MainWidget::doLayout()
{
    m_titlebar = new QWidget;
    m_titlebar->setFixedHeight(30);
    m_titlebar->setStyleSheet("QWidget{background-color: rgb(255, 0, 0)}");

    //中央区窗口分隔
    m_central = new QSplitter;
    m_central->setBackgroundRole(QPalette::Window);
    m_central->setHandleWidth(2);
    m_central->setOrientation(Qt::Horizontal);
    //中央区域布局
    this->centralLayout();

    m_statubar = new QWidget;
    m_statubar->setFixedHeight(30);
    m_statubar->setStyleSheet("QWidget{background-color: rgb(0, 0, 255)}");

    QHBoxLayout* m_lay_Main_t = new QHBoxLayout;
    m_lay_Main_t->addWidget(m_titlebar);

    QHBoxLayout* m_lay_Main_c = new QHBoxLayout;
    m_lay_Main_c->addWidget(m_central);

    QHBoxLayout* m_lay_Main_b = new QHBoxLayout;
    m_lay_Main_b->addWidget(m_statubar);

    QVBoxLayout* m_lay_Main = new QVBoxLayout;
    m_lay_Main->setMargin(0);
    m_lay_Main->setSpacing(0);
//    m_lay_Main->addLayout(m_lay_Main_t);
    m_lay_Main->addLayout(m_lay_Main_c);
//    m_lay_Main->addLayout(m_lay_Main_b);

    this->setLayout(m_lay_Main);
    this->resize(600,480);
}

void MainWidget::centralLayout()
{
    //中央区左侧
    QWidget* central_l = new QWidget;
    central_l->setLayout(new QVBoxLayout);
    central_l->layout()->setMargin(0);
    central_l->layout()->setSpacing(0);
    central_l->layout()->addWidget(createListWidget());
    central_l->layout()->addWidget(createTreeWidget());

    //中央区右侧
    QWidget* central_r = new QWidget;
    central_r->setStyleSheet("QWidget{background-color:white;}");
    central_r->setLayout(new QVBoxLayout);
    central_r->layout()->addWidget(createIconWidget());

    this->m_central->addWidget(central_l);
    this->m_central->addWidget(central_r);

    QList<int> list;
    list<<200<<700;
    this->m_central->setSizes(list);
}

QWidget* MainWidget::createListWidget()
{
    QListWidgetItem* dItem = new QListWidgetItem();
    dItem->setIcon(QIcon(QPixmap(":/toolButton/accessorywindow")));
    dItem->setData(Qt::DisplayRole,QVariant("桌面"));
    dItem->setData(Qt::WhatsThisRole,QVariant(QDesktopServices::storageLocation(QDesktopServices::DesktopLocation)));

    QListWidgetItem* pItem = new QListWidgetItem();
    pItem->setIcon(QIcon(QPixmap(":/toolButton/accessorywindow")));
    pItem->setData(Qt::DisplayRole,QVariant("程序目录"));
    pItem->setData(Qt::WhatsThisRole,QVariant(QCoreApplication::applicationDirPath()));

    lView = new QListWidget();
    lView->setStyleSheet("QListWidget{padding-top:5px;padding-left:18px;background-color:white;border:0;}");
    lView->setFlow(QListView::TopToBottom);
    lView->setFixedHeight(50);
    lView->addItem(dItem);
    lView->addItem(pItem);    
    connect(lView, SIGNAL(clicked(QModelIndex)),this, SLOT(slotListItemClicked(QModelIndex)));
    return lView;
}

QWidget* MainWidget::createTreeWidget()
{
    QFileSystemModel *fModel = new QFileSystemModel;
    fModel->setRootPath("");
    fModel->setFilter(QDir::Dirs | QDir::NoDotAndDotDot);

    tView = new QTreeView;
    tView->setStyleSheet("QTreeView{border:0;}");
    tView->setModel(fModel);
    tView->setAnimated(false);
    tView->setHeaderHidden(true);
    for(int i=1; i<fModel->columnCount(); i++){
        tView->setColumnHidden(i, true);
    }
    connect(tView,SIGNAL(clicked(QModelIndex)),this,SLOT(slotTreeItemClicked(QModelIndex)));
    return tView;
}

QWidget* MainWidget::createIconWidget()
{
    iView = new QListView();
    iView->setStyleSheet("QListView{background-color:white;border:0;}");
    iView->setSelectionMode(QAbstractItemView::MultiSelection);
    iView->setViewMode(QListView::IconMode);
    iView->setResizeMode(QListView::Adjust);
    iView->setMovement(QListView::Static);
    iView->setIconSize(QSize(120,120));
    iView->setGridSize(QSize(130,150));
    connect(iView,SIGNAL(clicked(QModelIndex)),this,SLOT(slotIconItemClicked(QModelIndex)));

    QAction* act = new QAction(iView);
    act->setShortcut(QKeySequence(Qt::Key_Escape));
    connect(act, SIGNAL(triggered()), SLOT(slotCancelSelected()));
    iView->addAction(act);

    return iView;
}

void MainWidget::setIconWidgetPath(QString path)
{
    QStandardItemModel* fModel = qobject_cast<QStandardItemModel*>(iView->model());
    if(!fModel)
    {
        fModel = new QStandardItemModel;
        iView->setModel(fModel);
    }
    fModel->clear();
    QStringList filter;
    filter<<QString("*.jpeg")<<QString("*.jpg")<<QString("*.png")<<QString("*.gif")<<QString("*.bmp");
    QDirIterator dir_iter(path, filter, QDir::Files | QDir::NoSymLinks, QDirIterator::NoIteratorFlags);
    if(dir_iter.hasNext()){
        while(dir_iter.hasNext()){
            dir_iter.next();
            QString file = dir_iter.fileInfo().absoluteFilePath();
            QImage img(file);
            if(!img.isNull()){
                QPixmap pix = QPixmap::fromImage(img);
                if(pix.size().width() > 300 || pix.size().height()>300){
                    pix = pix.scaled(200,200, Qt::KeepAspectRatio);    //,Qt::SmoothTransformation)
                }
                QStandardItem* item = new QStandardItem(QIcon(pix),file.mid(file.lastIndexOf("/")+1));
                item->setData(QVariant(file),Qt::WhatsThisRole);
                fModel->appendRow(item);
            }
        }
    }
}

void MainWidget::slotListItemClicked(QModelIndex index)
{
    tView->setCurrentIndex(tView->rootIndex());
    QString path = index.data(Qt::WhatsThisRole).toString();
    setIconWidgetPath(path);
}

void MainWidget::slotTreeItemClicked(QModelIndex index)
{
    lView->setCurrentIndex(lView->rootIndex());
    QModelIndex item = index;
    QString path = "";
    while(item.isValid()) {
        if(path.isEmpty()){
            path = item.data().toString();
        }else{
            path = item.data().toString().append("/").append(path);
        }
        item = item.parent();
    }
    setIconWidgetPath(path);
}

void MainWidget::slotIconItemClicked(QModelIndex)
{
    QModelIndexList mList = iView->selectionModel()->selectedIndexes();
    qDebug()<<mList.count();
}

void MainWidget::slotCancelSelected()
{
    iView->selectionModel()->clear();
    QModelIndexList mList = iView->selectionModel()->selectedIndexes();
    qDebug()<<mList.count();
}
