#ifndef VECTORRECT_H
#define VECTORRECT_H

#include <QDialog>
#include <QString>
QT_BEGIN_NAMESPACE
class ImageArea;
QT_END_NAMESPACE

namespace Ui {
    class vectorrect;
}

class vectorrect : public QDialog
{
    Q_OBJECT

public:
    explicit vectorrect(QWidget *parent = 0);
    void xmlrecta();
    void setcordi(int,int,int,int,QColor);
    void imgptr(ImageArea *a);
    void updateimg();
    ~vectorrect();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_horizontalSlider_valueChanged(int value);

    void on_horizontalSlider_2_valueChanged(int value);

    void on_pushButton_3_clicked();

private:
    Ui::vectorrect *ui;
    QString str1,str2;
    int xstart,ystart,height,width,w,fr,fg,fb,sr,sg,sb;
    float fo,so;
    QColor co;
    ImageArea *p;
};

#endif // VECTORRECT_H
