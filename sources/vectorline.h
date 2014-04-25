#ifndef VECTORLINE_H
#define VECTORLINE_H

#include <QDialog>
#include <QString>
QT_BEGIN_NAMESPACE
class ImageArea;
QT_END_NAMESPACE
namespace Ui {
    class vectorline;
}

class vectorline : public QDialog
{
    Q_OBJECT

public:
    explicit vectorline(QWidget *parent = 0);
    void setcordi(int,int,int,int,QColor);
    void xmlline();
    void imgptr(ImageArea *a);
    void updateimg();
    ~vectorline();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::vectorline *ui;
    int x11,y11,x22,y22,r,g,b,w;
    QString str1,str2;
    QColor co;
     ImageArea *p;
};

#endif // VECTORLINE_H
