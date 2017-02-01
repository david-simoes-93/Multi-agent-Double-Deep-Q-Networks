/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package charts;

import org.jfree.chart.ChartFactory;
import org.jfree.chart.ChartPanel;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.data.category.DefaultCategoryDataset;

import javax.swing.*;
import java.awt.*;

/**
 * @author bluemoon
 */
public class ChartFrame extends JFrame {

    public ChartFrame(String title, String a, String b, DefaultCategoryDataset ds) {
        JFreeChart lineChart = ChartFactory.createLineChart(
                title,
                a, b,
                ds,
                PlotOrientation.VERTICAL,
                true, true, false);
        ChartPanel af = new ChartPanel(lineChart);

        this.setLayout(new BorderLayout(0, 5));
        //JPanel panel = new JPanel(new FlowLayout(FlowLayout.RIGHT));
        //panel.add(chartPanel);
        this.add(af, BorderLayout.CENTER);

        this.pack();
        this.setLocationRelativeTo(null);
        this.setVisible(true);
    }

    public static void main(String[] args) {
        EventQueue.invokeLater(new Runnable() {
            @Override
            public void run() {
                ChartFrame cpd = new ChartFrame("Title", "x", "y", null);
            }
        });
    }
}
