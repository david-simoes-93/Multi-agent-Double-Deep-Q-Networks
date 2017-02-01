package utils;

import java.util.Arrays;

/**
 * Created by david on 03-11-2016.
 */

public class Statistics
{
    double[] data;
    int size;

    public Statistics(double[] data)
    {
        this.data = data;
        size = data.length;
    }

    public Statistics(Object[] data)
    {
        this.data=new double[data.length];
        for(int i=0; i<data.length; i++)
            this.data[i] = (double)data[i];
        size = data.length;
    }

    public double getMean()
    {
        double sum = 0.0;
        for(double a : data)
            sum += a;
        return sum/size;
    }

    double getVariance()
    {
        double mean = getMean();
        double temp = 0;
        for(double a :data)
            temp += (a-mean)*(a-mean);
        return temp/size;
    }

    double getUVariance()
    {
        double mean = getMean();
        double temp = 0;
        int size=0;
        for(double a :data)
            if(a>=mean) {
                temp += (a - mean) * (a - mean);
                size++;
            }
        return temp/size;
    }

    double getLVariance()
    {
        double mean = getMean();
        double temp = 0;
        int size=0;
        for(double a :data)
            if(a<=mean) {
                temp += (a - mean) * (a - mean);
                size++;
            }
        return temp/size;
    }

    public double getStdDev()
    {
        return Math.sqrt(getVariance());
    }

    public double getUStdDev()
    {
        return Math.sqrt(getUVariance());
    }
    public double getLStdDev()
    {
        return Math.sqrt(getLVariance());
    }


    public double median()
    {
        Arrays.sort(data);

        if (data.length % 2 == 0)
        {
            return (data[(data.length / 2) - 1] + data[data.length / 2]) / 2.0;
        }
        else
        {
            return data[data.length / 2];
        }
    }

    public double lQuartile()
    {
        Arrays.sort(data);

        if (data.length % 2 == 0)
        {
            return (data[(data.length / 8) - 1] + data[data.length / 8]) / 2.0;
        }
        else
        {
            return data[data.length / 8];
        }
    }
    public double uQuartile()
    {
        Arrays.sort(data);

        if (data.length % 2 == 0)
        {
            return (data[(data.length*7 / 8) - 1] + data[data.length*7 / 8]) / 2.0;
        }
        else
        {
            return data[data.length*7 / 8];
        }
    }
}