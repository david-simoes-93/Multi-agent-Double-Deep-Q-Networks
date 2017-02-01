package utils;

/**
 * Created by david on 09-11-2016.
 */

import javax.imageio.ImageIO;
import javax.imageio.stream.FileImageOutputStream;
import javax.imageio.stream.ImageOutputStream;
import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.util.Scanner;

public class MakeGif {
    public static void main(String[]args){
        try {
            int size=15, scale=100;
            int width = size*scale, height = size*scale;

            // TYPE_INT_ARGB specifies the image format: 8-bit RGBA packed
            // into integer pixels


            String folder = "prstGif/";
            Scanner fin = new Scanner(new File(folder+"game.log")); //file

            StringBuilder map;
            String prevMap="";
            int counter=0;
            while(fin.hasNextLine()){
                String f = fin.nextLine();
                //boolean justOut=false;
                if(f.contains("(world (")){
                    System.out.println(f);
                    map = new StringBuilder();
                    for(int i=0; i<size; i++){
                        for(int j=0; j<size; j++){
                            map.append(" ");
                        }
                        map.append("\n");
                    }
                    //map = new StringBuilder("       \n       \n       \n       \n       \n       \n       \n");
                    String [] info = f.split("\\(\\(");
                    //System.out.println(map.toString()+"\n-");
                    for(int i=1; i<info.length; i++){
                        String [] temp = info[i].replaceAll("\\)", "").split(" ");
                        if(temp[0].equals("prey") && !temp[1].contains("x")){
                            map.setCharAt(Integer.parseInt(temp[temp.length-2])+
                                    Integer.parseInt(temp[temp.length-1])*size, 'O');
                        }else if(temp[0].equals("predator")) {
                            map.setCharAt(Integer.parseInt(temp[temp.length-2])+
                                    Integer.parseInt(temp[temp.length-1])*size, 'H');
                        }
                    //    System.out.println(map.toString()+"\n-");
                    }

                    if(!map.equals(prevMap)) {
                        BufferedImage bi = new BufferedImage(width, height, BufferedImage.TYPE_INT_ARGB);
                        Graphics2D ig2 = bi.createGraphics();
                        //System.out.println(map);
                        for (int y = 0; y < size; y++) {
                            for (int x = 0; x < size; x++) {
                                ig2.setPaint(getColor(map.charAt(x + y * size)));
                                ig2.fillRect(x * scale, y * scale, 1 * scale, 1 * scale);
                            }
                        }
                        ImageIO.write(bi, "PNG", new File("prstGif/img" + counter + ".PNG"));
                        counter++;
                    }

                    prevMap=map.toString();

                }
            }

            int gifCounter=0;
            // grab the output image type from the first image in the sequence
            BufferedImage firstImage = ImageIO.read(new File("prstGif/img" + gifCounter + ".PNG"));

            // create a new BufferedOutputStream with the last argument
            ImageOutputStream output =
                    new FileImageOutputStream(new File("prstGif/gif.GIF"));

            // create a gif sequence with the type of the first image, 1 second
            // between frames, which loops continuously
            GifSequenceWriter writer = new GifSequenceWriter(output, firstImage.getType(), 200, false);

            // write out the first image to our sequence...
            writer.writeToSequence(firstImage);
            for(gifCounter=1; gifCounter<counter; gifCounter++) {
                //System.out.println(gifCounter);
                BufferedImage nextImage = ImageIO.read(new File("prstGif/img" + gifCounter + ".PNG"));
                writer.writeToSequence(nextImage);
            }

            writer.close();
            output.close();
        } catch (IOException ie) {
            ie.printStackTrace();
        }
    }

    public static void main2(String[]args){
        try {
            int size=7, scale=100;
            int width = size*scale, height = size*scale;

            // TYPE_INT_ARGB specifies the image format: 8-bit RGBA packed
            // into integer pixels


            String folder = "7x7gen_1/";
            Scanner fin = new Scanner(new File(folder+"10b"));

            boolean inMap=false;
            String map="";
            String prevMap="";
            int counter=0;
            while(fin.hasNextLine()){
                String f = fin.nextLine();
                boolean justOut=false;
                if(f.contains(" - X")){
                    boolean justStarted=false;
                    if(!inMap && f.contains(" - XXXXXXX")){
                        inMap=true;
                        justStarted=true;
                    }
                    if(inMap)
                        map+=f.split(" - ")[1];

                    if(inMap && !justStarted && f.contains(" - XXXXXXX")){
                        inMap=false;
                        justOut=true;
                    }
                }

                if(justOut){
                    if(!map.equals(prevMap)) {
                        BufferedImage bi = new BufferedImage(width, height, BufferedImage.TYPE_INT_ARGB);
                        Graphics2D ig2 = bi.createGraphics();
                        //System.out.println(map);
                        for (int y = 0; y < size; y++) {
                            for (int x = 0; x < size; x++) {
                                ig2.setPaint(getColor(map.charAt(x + y * 7)));
                                ig2.fillRect(x * scale, y * scale, 1 * scale, 1 * scale);
                            }
                        }
                        ImageIO.write(bi, "PNG", new File("Gif/img" + counter + ".PNG"));
                        counter++;
                    }

                    prevMap=map;
                    map="";
                }
            }

            int gifCounter=0;
            // grab the output image type from the first image in the sequence
            BufferedImage firstImage = ImageIO.read(new File("Gif/img" + gifCounter + ".PNG"));

            // create a new BufferedOutputStream with the last argument
            ImageOutputStream output =
                    new FileImageOutputStream(new File("Gif/gif.GIF"));

            // create a gif sequence with the type of the first image, 1 second
            // between frames, which loops continuously
            GifSequenceWriter writer = new GifSequenceWriter(output, firstImage.getType(), 1, false);

            // write out the first image to our sequence...
            writer.writeToSequence(firstImage);
            for(gifCounter=1; gifCounter<counter; gifCounter++) {
                //System.out.println(gifCounter);
                BufferedImage nextImage = ImageIO.read(new File("Gif/img" + gifCounter + ".PNG"));
                writer.writeToSequence(nextImage);
            }

            writer.close();
            output.close();
        } catch (IOException ie) {
            ie.printStackTrace();
        }
    }

    public static Color getColor(char c){
        if(Character.isDigit(c))
            return Color.BLUE;

        switch(c){
            case 'X':
                return Color.black;     //obstacle
            case '=':
                return Color.BLUE;      // otheragent
            case 'Ü':
            case 'Ö':
                return new Color(0,128,255); //Color.cyan;      // agent on berry
            case 'ü':
            case 'ö':
                return new Color(0,255,128);      // carrying on berry
            case 'O':
                return Color.green;     // berry
            case ' ':
                return Color.white;     // empty
            case 'Ä':
            case 'Ë':
                return new Color(255,0,255); //Color.YELLOW;    // agent at base
            case 'ä':
            case 'ë':
                return new Color(125,128,200);      // carrying at base
            case 'E':
                return Color.RED;       // base
            case 'u':
            case 'h':
                return new Color(0,255,255); // carrying
            case 'U':
            case 'H':
                return Color.BLUE;          // agent
            default:
                return Color.white;
        }
    }
}
