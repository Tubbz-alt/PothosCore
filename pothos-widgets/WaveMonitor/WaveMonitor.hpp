// Copyright (c) 2014-2014 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#pragma once
#include <qwt_math.h> //_USE_MATH_DEFINES
#include <Pothos/Framework.hpp>
#include <QWidget>
#include <memory>
#include <chrono>
#include <valarray>
#include <map>
#include <vector>

class MyQwtPlot;
class QwtPlotGrid;
class QwtPlotCurve;
class QwtPlotZoomer;
class QwtPlotMarker;

/***********************************************************************
 * |PothosDoc Wave Monitor
 *
 * The wave monitor plot displays a live two dimensional plot of input elements vs time.
 *
 * |category /Widgets
 * |keywords time plot wave scope
 *
 * |param numInputs[Num Inputs] The number of input ports.
 * |default 1
 * |widget SpinBox(minimum=1)
 * |preview disable
 *
 * |param title The title of the plot
 * |default "Amplitude vs Time"
 * |widget StringEntry()
 *
 * |param displayRate[Display Rate] How often the plotter updates.
 * |default 10.0
 * |units updates/sec
 *
 * |param sampleRate[Sample Rate] The rate of the input elements.
 * |default 1e6
 * |units samples/sec
 *
 * |param numPoints[Num Points] The number of points per plot capture.
 * |default 1024
 *
 * |param enableXAxis[Enable X-Axis] Show or hide the horizontal axis markers.
 * |option [Show] true
 * |option [Hide] false
 * |default true
 * |preview disable
 *
 * |param enableYAxis[Enable Y-Axis] Show or hide the vertical axis markers.
 * |option [Show] true
 * |option [Hide] false
 * |default true
 * |preview disable
 *
 * |param yAxisTitle[Y-Axis Title] The title of the verical axis.
 * |default ""
 * |widget StringEntry()
 * |preview disable
 *
 * |param rateLabelId[Rate Label ID] Labels with this ID can be used to set the sample rate.
 * To ignore sample rate labels, set this parameter to an empty string.
 * |default "rxRate"
 * |widget StringEntry()
 * |preview disable
 * |tab Labels
 *
 * |mode graphWidget
 * |factory /widgets/wave_monitor()
 * |initializer setNumInputs(numInputs)
 * |setter setTitle(title)
 * |setter setDisplayRate(displayRate)
 * |setter setSampleRate(sampleRate)
 * |setter setNumPoints(numPoints)
 * |setter enableXAxis(enableXAxis)
 * |setter enableYAxis(enableYAxis)
 * |setter setYAxisTitle(yAxisTitle)
 * |setter setRateLabelId(rateLabelId)
 **********************************************************************/
class WaveMonitor : public QWidget, public Pothos::Block
{
    Q_OBJECT
public:

    static Block *make(void)
    {
        return new WaveMonitor();
    }

    WaveMonitor(void);

    ~WaveMonitor(void);

    QWidget *widget(void)
    {
        return this;
    }

    void setNumInputs(const size_t numInputs);

    //! set the plotter's title
    void setTitle(const QString &title);

    /*!
     * update rate for the plotter
     * how often to update the display
     */
    void setDisplayRate(const double displayRate);

    /*!
     * sample rate for the plotter
     * controls the time scaling display
     */
    void setSampleRate(const double sampleRate);

    void setNumPoints(const size_t numPoints);

    QString title(void) const;

    size_t numInputs(void) const
    {
        return this->inputs().size();
    }

    double displayRate(void) const
    {
        return _displayRate;
    }

    double sampleRate(void) const
    {
        return _sampleRate;
    }

    size_t numPoints(void) const
    {
        return _numPoints;
    }

    void enableXAxis(const bool enb);
    void enableYAxis(const bool enb);
    void setYAxisTitle(const QString &title);

    void setRateLabelId(const std::string &id)
    {
        _rateLabelId = id;
    }

    void activate(void);
    void work(void);
    bool updateCurve(Pothos::InputPort *inPort);

    //allow for standard resize controls with the default size policy
    QSize minimumSizeHint(void) const
    {
        return QSize(300, 150);
    }
    QSize sizeHint(void) const
    {
        return this->minimumSizeHint();
    }

private slots:
    void installLegend(void);
    void handleLegendChecked(const QVariant &, bool, int);
    void handleSamples(const int index, const int curve, const std::valarray<float> &samps, const std::vector<Pothos::Label> &labels);
    void handleUpdateAxis(void);
    void handleZoomed(const QRectF &rect);

private:
    MyQwtPlot *_mainPlot;
    QwtPlotGrid *_plotGrid;
    QwtPlotZoomer *_zoomer;
    double _displayRate;
    double _sampleRate;
    double _timeSpan;
    size_t _numPoints;
    std::string _rateLabelId;

    //per-port data structs
    std::map<size_t, std::chrono::high_resolution_clock::time_point> _lastUpdateTimes;
    std::map<size_t, std::vector<Pothos::BufferChunk>> _rasterBuffs;
    std::map<size_t, std::map<size_t, std::shared_ptr<QwtPlotCurve>>> _curves;
    std::map<size_t, std::vector<std::shared_ptr<QwtPlotMarker>>> _markers;
    size_t _nextColorIndex;
};