import java.awt.*;
import javax.swing.*;
import java.awt.event.*;
import javax.imageio.*;
import java.io.*;

public class Fish extends JPanel{

    int pos_x;
    int pos_y;
    int width;
    int height;
    String name;
    String type;
    Image image;

    public Fish(String name, String type,int pos_x,int pos_y,int width,int height){
	this.name = name;
	this.type = type;
	this.pos_x = pos_x;
	this.pos_y = pos_y;	
	this.width = width;
	this.height = height;
	
	try{
	    this.image = ImageIO.read(new File("fishes/magicarpe.png"));
	} catch (IOException e){
	    System.err.println("Cannot read image of type " + type);
	}

	this.setBounds(this.pos_x, this.pos_y, this.width, this.height);
	this.setOpaque(false);
    }

    public String getName(){
	return name;
    }

    public String getType(){
	return type;
    }

    public int getPosX(){
	return pos_x;
    }
    
    public int getPosY(){
	return pos_y;
    }

    public int getWidth(){
	return width;
    }

    public int getHeight(){
	return height;
    }

    public void setPosX(int newPosX){
	pos_x = newPosX;
    }

    public void setPosY(int newPosY){
	pos_y = newPosY;
    }

    public void move(){
    }
    
    @Override
    public void paintComponent(Graphics g) {
	super.paintComponent(g);
	this.setBounds(this.pos_x, this.pos_y, this.width, this.height);
	g.drawImage(image, 0, 0, this.width , this.height, this);
    }

    @Override
    public String toString(){
	return "nom: " + name + " type: " + type + " pos: " + pos_x + ", " + pos_y + " taille: " + width + ", " + height;
    }
    
}
