#include "mainwidget.h"
#include <QApplication>
#include <qfont.h>
#include <stdio.h>
#include <stdlib.h>
#include <QMutex>
#include <QDateTime>
#include <QMessageLogContext>
#include <QMessageBox>
#include <qdebug.h>
#include <QSharedMemory>
#include "netconfig.h"


void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg);
void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qInstallMessageHandler(outputMessage); //注册MsgHandler回调函数
    QFile file(QCoreApplication::applicationDirPath()+"/"+"netdisk"+".log");
    file.open(QIODevice::WriteOnly);
    file.close();
    qDebug("Netdisk start");
    QFont font;
    font.setFamily("微软雅黑");
    font.setPointSize(9);
    a.setFont(font);


    QSharedMemory shared_memory;
    shared_memory.setKey(QString("main_window"));
    if(shared_memory.attach())
    {
//        QMessageBox::about(NULL, "联瑞企业网盘","联瑞企业网盘已启动");
        return 0;
    }

    if(shared_memory.create(1))
    {
        netConf = new NetConfig;
        MainWidget w;
        return a.exec();
    //    w.show();
    }
    return 0;
}

void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QMutex mutex;
    mutex.lock();
    QtMsgType msgType = type;
    QString text;
    switch(msgType)
    {
    case QtDebugMsg:
        text = QString("Debug:");
        break;

    case QtWarningMsg:
        text = QString("Warning:");
        break;

    case QtCriticalMsg:
        text = QString("Critical:");
        break;

    case QtFatalMsg:
        text = QString("Fatal:");
    }

    QString context_info = QString("File:(%1) Line:(%2)").arg(QString(context.file)).arg(context.line);
    QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
    QString current_date = QString("(%1)").arg(current_date_time);
    QString message = QString("%1 %2 %3 %4").arg(text).arg(context_info).arg(msg).arg(current_date);

    QFile file(QCoreApplication::applicationDirPath()+"/"+"netdisk"+".log");
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream text_stream(&file);
    text_stream << message << "\r\n";
    file.flush();
    file.close();

    mutex.unlock();
}

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
  {
      QByteArray localMsg = msg.toLocal8Bit();
      switch (type) {
      case QtDebugMsg:
          fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
          break;
      case QtInfoMsg:
          fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
          break;
      case QtWarningMsg:
          fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
          break;
      case QtCriticalMsg:
          fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
          break;
      case QtFatalMsg:
          fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
          abort();
      }
  }

