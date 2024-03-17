#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Ui::MainWindow *ui;
    bool ping(const char*);

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void updateLabelText();

private slots:
    void on_close_clicked();

    void on_envoie_clicked();

    void on_recup_clicked();

    void on_pushButton_clicked();   

};
#endif // MAINWINDOW_H
