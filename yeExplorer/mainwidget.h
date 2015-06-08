#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QSplitter>
#include <QListWidget>
#include <QTreeView>
#include <QFileSystemModel>

class MainWidget : public QWidget
{
    Q_OBJECT
    
public:
    MainWidget(QWidget *parent = 0);
    ~MainWidget();



private:
    void doLayout();
    void centralLayout();
    QWidget* createListWidget();
    QWidget* createTreeWidget();
    QWidget* createIconWidget();
    void setIconWidgetPath(QString path);

private slots:
    void slotListItemClicked(QModelIndex index);
    void slotTreeItemClicked(QModelIndex index);
    void slotIconItemClicked(QModelIndex);
    void slotCancelSelected();

private:
    QWidget* m_titlebar;
    QSplitter* m_central;
    QWidget* m_statubar;

    QTreeView* tView;
    QListView* iView;
    QListWidget* lView;
};

#endif // MAINWIDGET_H
