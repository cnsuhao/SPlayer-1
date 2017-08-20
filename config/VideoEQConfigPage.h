#pragma once

#include <QWidget>

QT_BEGIN_NAMESPACE
class QCheckBox;
class QComboBox;
class QPushButton;
class QSlider;
QT_END_NAMESPACE
class VideoEQConfigPage : public QWidget
{
    Q_OBJECT
public:
    enum Engine {
        SWScale,
        GLSL,
        XV,
    };
    explicit VideoEQConfigPage(QWidget *parent = 0);
    void setEngines(const QVector<Engine>& engines);
    void setEngine(Engine engine);
    Engine engine() const;

    qreal brightness() const;
    qreal contrast() const;
    qreal hue() const;
    qreal saturation() const;

signals:
    void engineChanged();
    void brightnessChanged(int);
    void contrastChanged(int);
    void hueChanegd(int);
    void saturationChanged(int);

private slots:
    void onGlobalSet(bool);
    void onReset();
    void onEngineChangedByUI();

private:
    QCheckBox *mpGlobal;
    QComboBox *mpEngine;
    QSlider *mpBSlider, *mpCSlider, *mpSSlider;
    QSlider *mpHSlider;
    QPushButton *mpResetButton;
    Engine mEngine;
    QVector<Engine> mEngines;
};
