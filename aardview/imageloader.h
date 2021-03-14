/**
 * @file imageloader.h
 * @copyright GPLv2
 * @author Bernd Wachter <bwachter@lart.info>
 * @date 2009-2016
 */

#ifndef _IMAGELOADER_H
#define _IMAGELOADER_H

#include <QPixmap>
#include <QDebug>

#ifdef HAS_EXIF
#include <libexif/exif-data.h>
#define MAX_EXIF_BUFFER 1024
#endif

class ImageLoader: public QObject {
    Q_OBJECT

  public:
    ImageLoader();
    /**
     * Return the filename of the currently loaded image.
     */
    QString currentFilename() const;
    /**
     * Return the currently loaded pixmap, unscaled. This method does not
     * check if a pixmap is loaded, and will return a null pixmap in that case.
     */
    QPixmap currentPixmap() const;

  private:
    /// Flag storing if the image should be fitted to view size
    bool m_fitToWindow;
    /// The scale factor applied to the image for displaying
    double m_scaleFactor;
    /// The raw pixmap data (original size)
    QPixmap m_pixmap;
    /// The filename (including path) of the current image
    QString m_imageFileName;
    /// The size of the display widget (i.e., size the image is scaled to)
    QSize m_viewSize;
    QHash<QString, QString> m_exifData;

    /// The transformation mode for scaling the image (fast or smooth)
    Qt::TransformationMode m_transformation;

    /**
     * Display exif data for current image, if possible.
     */
    void displayEXIF();

    /**
     * Scale the image correctly, and emit pixmapReady() once done.
     */
    void displayImage();
    /**
     * Apply scale factor and re-display image.
     *
     * The provided scale factor is multied with the stored scale factor,
     * and then stored for later scaling operations. Use a scale factor of
     * 0 to reset scaling.
     *
     * @param factor scale factor to apply
     */
    void scale(double factor);
#ifdef HAS_EXIF
    static void exifDataContentCB(ExifContent *content, void *user_data);
    static void exifDataEntryCB(ExifEntry *entry, void *user_data);
#endif

  public slots:
    /**
     * Load the given image, scale it to provided view size, and repaint.
     *
     * @param pathname the path to the image to load
     * @param widgetViewSize the view size of the display widget
     */
    void load(const QString &pathname, const QSize &widgetViewSize);
    /**
     * Toggle fitting the image to the view size and repaint.
     */
    void toggleFtw();
    /**
     * Scale image to its native size and repaint.
     */
    void normalSize();
    /**
     * Re-read settings and apply them. This slot usually should be attached to
     * a configurationChanged() signal of a settings object.
     */
    void reconfigure();
    /**
     * Adjust the output picture size and repaint. This slot usually should
     * be called from resize events of the display widgets.
     *
     * @param widgetViewSize the new view size
     */
    void repaint(const QSize &widgetViewSize);
    /**
     * Rotate the image by 90 degrees clockwise.
     * This is currently not implemented.
     */
    void rotate();
    /**
     * Zoom into the image by factor 1.25 to the current displayed size
     */
    void zoomIn();
    /**
     * Zoom out of the image by factor 0.8 to the current displayed size
     */
    void zoomOut();

  private slots:

  signals:
    /**
     * Pass a properly scaled pixmap of the current image once loaded
     */
    void pixmapReady(const QPixmap &picture);
};


#endif
