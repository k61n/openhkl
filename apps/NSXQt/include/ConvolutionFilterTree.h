#include <QTreeWidget>
#include <QComboBox>

class ConvolutionFilterTree: public QTreeWidget {
    Q_OBJECT

private:
    QComboBox* m_comboBox;

public:
    ConvolutionFilterTree(QWidget* parent);


public slots:
    void onComboBoxChange();
};

