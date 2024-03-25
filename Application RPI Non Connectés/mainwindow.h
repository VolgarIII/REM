#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <filesystem>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Ui::MainWindow *ui;
    bool ping(const char*);
    std::filesystem::path cheminFichierREM = std::filesystem::current_path()/ "dossierREM";

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void updateLabelText();
    void verifierSauvegarde();

private slots:
    void on_close_clicked();

    void on_envoie_clicked();

    void on_recup_clicked();

    void on_effacerSauvegarde_clicked();

};
#endif // MAINWINDOW_H
