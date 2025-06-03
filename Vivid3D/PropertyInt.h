#include <QLabel>
#include <QDoubleSpinBox>
class PropertyInt : public QWidget
{
    Q_OBJECT

public:
    PropertyInt(const QString& label, int minValue, int maxValue,int interval, QWidget* parent = nullptr);
    ~PropertyInt();
    void setValue(int value);
    int getValue() const;

signals:
    void valueChanged(int value);

private slots:
    void onSpinBoxChanged();

private:
    void setupSpinBox();


    //Ui::PropertyFloatClass ui;
    QLabel* m_label;
    QSpinBox* m_spinBox;
    int m_minValue;
    int m_maxValue;
    int m_interval;
};

