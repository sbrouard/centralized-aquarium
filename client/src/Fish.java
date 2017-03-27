import java.awt.*;
import javax.swing.*;
import java.awt.event.*;
import javax.imageio.*;
import java.io.*;
import java.lang.Math;

public class Fish extends JPanel{

    private int pos_x;
    private int pos_y;
    private int toGo_x;
    private int toGo_y;
    private int from_x;
    private int from_y;
    private float time; // time to go to pos (toGo_x, toGo_y)
    private int width;
    private int height;
    private String name;
    private String type;
    private Image image;
    private int nbTimesUpdated;

    public Fish(String name, String type,int pos_x,int pos_y,int width,int height, float time){
	this.name = name;
	this.type = type;
	this.pos_x = pos_x;
	this.pos_y = pos_y;
	this.toGo_x = pos_x;
	this.toGo_y = pos_y;
	this.from_x = pos_x;
	this.from_y = pos_y;
	this.time = time;
	this.width = width;
	this.height = height;
	this.nbTimesUpdated = 0;
	
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

    public void setTime(int time){
	this.time = time;
    }
    
    public float getTime(){
	return this.time;
    }

    public int getToGoX(){
	return this.toGo_x;
    }

    public int getToGoY(){
	return this.toGo_y;
    }

    public void moveStraight(){

	if(this.pos_x != this.toGo_x && this.time != 0 && 50*nbTimesUpdated < 1000*this.time){
	    this.pos_x = (int)(((float)(this.toGo_x - this.from_x) / (this.time*1000))*50*(nbTimesUpdated+1)) + this.from_x;
	}

	if(this.pos_y != this.toGo_y && this.time != 0 && 50*nbTimesUpdated < 1000*this.time){
	    this.pos_y = (int)(((float)(this.toGo_y - this.from_y) / (this.time*1000))*50*(nbTimesUpdated+1)) + this.from_y;
	}

	if(this.pos_x == this.toGo_x || this.time == 0 || 50*nbTimesUpdated >= 1000*this.time){
	    this.pos_x = toGo_x;
	    this.from_x = this.pos_x;
	}

	if(this.pos_y == this.toGo_y || this.time == 0 || 50*nbTimesUpdated >= 1000*this.time){
	    this.pos_y = toGo_y;
	    this.from_y = this.pos_y;
	}
	
	System.out.println(this.pos_x + " " + this.pos_y);
	
	this.nbTimesUpdated++;
    }

    public void moveSin(){
	if(this.toGo_x != this.pos_x){
	    double teta = Math.atan((this.toGo_y - this.pos_y)/(this.toGo_x - this.pos_x)); 
	    System.out.println("angle: " + teta);  
	}
    }

    public void move(){
	this.moveStraight();
    }

    
    public void updateFish(Fish newFish){
	if(this.toGo_x != newFish.getToGoX() || this.toGo_y != newFish.getToGoY()){
	    this.time = newFish.getTime();
	    this.toGo_x = newFish.getToGoX();
	    this.toGo_y = newFish.getToGoY();
	    this.nbTimesUpdated = 0;
	    this.from_x = this.pos_x;
	    this.from_y = this.pos_y;
	}
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
