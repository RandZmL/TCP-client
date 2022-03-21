#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(readyRead()), this, SLOT(sockReady()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(sockDisc()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::sockReady()
{
    if(socket->waitForConnected(500))
    {
        socket->waitForReadyRead(11500);
        Data = socket->readAll();

        doc = QJsonDocument::fromJson(Data, &docError);

        if (docError.errorString() == "no error occurred")
        {
            if((doc.object().value("type").toString() == "connect") && (doc.object().value("status").toString() == "yes"))
            {
                QMessageBox::information(this, "Подключено", "Вы подключены к серверу!");
            }
            else if (doc.object().value("type").toString()=="resultSelect")
            {
                QStandardItemModel *model = new QStandardItemModel(nullptr);
                model->setHorizontalHeaderLabels(QStringList()<<"Name");

                QJsonArray docAr = doc.object().value("result").toArray();
                for (int i = 0; i < docAr.count(); i++ )
                {
                    QStandardItem *col = new QStandardItem(docAr[i].toObject().value("name").toString());
                    model->appendRow(col);

                }
                ui->tableView->setModel(model);
            }
            else
            {
                QMessageBox::warning(this, "Ошибка", "Подключение не установлено");
            }
        }
        else
        {
            QMessageBox::warning(this, "Ошибка", "Ошибка в формате передачи данных:" + docError.errorString());
        }
    }
}

void MainWindow::sockDisc()
{
    socket->deleteLater();
}


void MainWindow::on_connect_button_clicked()
{
    socket->connectToHost("127.0.0.1", 1234);
    ui->pushButton->setEnabled(true);
    ui->connect_button->setEnabled(false);
}

void MainWindow::on_pushButton_clicked()
{
    if (socket->isOpen())
    {
        socket->write("{\"type\":\"select\",\"params\":\"workers\"}");
        socket->waitForBytesWritten(500);
    }
    else
    {
        QMessageBox::warning(this, "Ошибка", "Соеденение не было установлено");
    }
}
