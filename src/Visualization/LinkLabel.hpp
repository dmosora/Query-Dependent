#ifndef LINKLABEL_HPP
#define LINKLABEL_HPP

#include <QLabel>
#include <QString>
#include <QMouseEvent>
#include <QPixmap>

/**
 * The LinkLabel class is a subclass of QLabel that adds mouse-click
 * handling to the label and also sets a hyperlink style for text labels.
 */
class LinkLabel : public QLabel
{
Q_OBJECT
public:
    /** Constructors for text labels as well as clickable images */
    LinkLabel(const QString& text, QWidget* parent = 0);
    LinkLabel(const QString& text, QString tooltip, QWidget* parent = 0);
    LinkLabel(const QPixmap& upImg, const QPixmap& downImg, QWidget* parent = 0);
    LinkLabel(const QPixmap& upImg, const QPixmap& downImg, QString text, QWidget* parent = 0);

    /** Accessors */
    bool isUp() { return !_down; }
    bool isDown() { return _down; }
    bool isText() { return _type; }
    bool isImage() { return !_type; }
    bool isCheckable() { return _checkable; }
    bool isChecked() { return _down; }
    bool isEnabled() { return _enabled; }
    bool isHyperlinkStyle() { return _hyperlink; }

    /** Modifiers */
    void setCheckable(bool checkable) { _checkable = checkable; }
    void setChecked(bool checked);
    void setState(bool up);
    void setEnabled(bool enabled) { _enabled = enabled; }
    void enableHyperlinkStyle(bool enable);
    void newImage(const QPixmap& upImg, const QPixmap& downImg);
    void newImage(const QPixmap& upImg, const QPixmap& downImg, QString text);
    void newToolTip(QString text) { _tooltip = text; }
    void scaleToHeight(int height);
    void scaleToWidth(int width);

signals:
    /** The clicked signal is emitted when a user clicks on the label. */
    void clicked();
    void clicked(LinkLabel*);

    /** The toggled signal is emitted when a user clicks a checkable label */
    void toggled(bool);

protected:
    virtual bool event(QEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void enterEvent(QEvent*);
    virtual void leaveEvent(QEvent*);
    virtual void toolTipEvent(QHelpEvent* event);

private:
    bool _enabled;
    bool _down;
    bool _checkable;
    bool _type;             // true = text, false = image.
    bool _hyperlink;

    QPixmap _upImg;
    QPixmap _downImg;
    QString _tooltip;
};

#endif // LINKLABEL_HPP
