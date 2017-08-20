#pragma once

#include <QObject>
#include <QMetaProperty>
#include <QMetaEnum>

QT_BEGIN_NAMESPACE
class QAction;
class QWidget;
QT_END_NAMESPACE
class PropertyEditor : public QObject
{
    Q_OBJECT
public:
    explicit PropertyEditor(QObject *parent = 0);
    // call it before others
    void getProperties(QObject *obj);

    // from config file etc to init properties. call it before buildXXX
    void set(const QVariantHash& hash);
    void set(const QString& conf);
    /*!
     * \brief buildOptions
     * \return command line options
     */
    QString buildOptions();
    QWidget* buildUi(QObject* obj = 0); //obj: read dynamic properties("detail_property")
    QVariantHash exportAsHash();
    QString exportAsConfig(); //json like

private:
    /*!
     * name is property name.
     * 1. add property and value in hash
     * 2. add a widget and set value
     * 3. connect widget value change signal to a slot
     */
    QWidget* createWidgetForFlags(const QString& name, const QVariant& value, QMetaEnum me, const QString& detail = QString(), QWidget* parent = 0);
    QWidget* createWidgetForEnum(const QString& name, const QVariant& value, QMetaEnum me, const QString& detail = QString(), QWidget* parent = 0);
    QWidget* createWidgetForInt(const QString& name, int value, const QString& detail = QString(), QWidget* parent = 0);
    QWidget* createWidgetForReal(const QString& name, qreal value, const QString& detail = QString(), QWidget* parent = 0);
    QWidget* createWidgetForText(const QString& name, const QString& value, bool readOnly, const QString& detail = QString(), QWidget* parent = 0);
    QWidget* createWidgetForBool(const QString& name, bool value, const QString& detail = QString(), QWidget* parent = 0);

    // called if value changed by ui (in onXXXChange)
    void updatePropertyValue(const QString& name, const QVariant& value);
private slots:
    // updatePropertyValue
    void onFlagChange(QAction *action);
    void onEnumChange(int value);
    void onIntChange(int value);
    void onRealChange(qreal value);
    void onTextChange(const QString& value);
    void onBoolChange(bool value);

private:
    QList<QMetaProperty> mMetaProperties;
    QVariantHash mProperties;
    QVariantHash mPropertyDetails;
};
