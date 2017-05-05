import java.awt.*;
import javax.swing.*;
import java.awt.event.*;
import javax.imageio.*;
import java.io.*;
import java.lang.Math;
import java.util.concurrent.locks.ReentrantLock;

public class Fish extends JPanel{

    private int pos_x;
    private int pos_y;
    
    private int toGo_x;
    private int toGo_y;
    
    private int from_x;
    private int from_y;
   
    private float time; // time to go to pos (toGo_x, toGo_y)
    
    private int p_width;
    private int p_height;
    
    private String name;
    private Image image;
    
    private int nbTimesUpdated;
    private final int timeUpdate;
    
    private double screenSizeHeight;
    private double screenSizeWidth;

    private ReentrantLock fishLock = new ReentrantLock();

    public Fish(String name, int pos_x, int pos_y, int width, int height, float time){
	this.name = name;
	this.pos_x = pos_x;
	this.pos_y = pos_y;
	this.toGo_x = this.pos_x;
	this.toGo_y = this.pos_y;
	this.from_x = this.pos_x;
	this.from_y = this.pos_y;
	this.time = time;
	this.p_width = width;
	this.p_height = height;
	this.nbTimesUpdated = 0;
	this.timeUpdate = 50;
	
	String[] type = name.split("_");
	setImage(type[0]);

	this.setBounds(this.pos_x, this.pos_y, this.p_width, this.p_height);
	this.setOpaque(false);
    }
    
    private void setImage(String type){
	try{
	    if("leviator".equals(type)){
		this.image = ImageIO.read(new File("fishes/leviator.png"));
	    } 
	    else if("poissirene".equals(type)){
		this.image = ImageIO.read(new File("fishes/poissirene.png"));
	    }
	    else if("barpau".equals(type)){
		this.image = ImageIO.read(new File("fishes/barpau.png"));
	    }
	    else if("barbicha".equals(type)){
		this.image = ImageIO.read(new File("fishes/barbicha.png"));
	    }
	    else {
		this.image = ImageIO.read(new File("fishes/magicarpe.png"));
	    }
	} catch (IOException e){
	    System.err.println("Cannot read image " + type + ".png");
	}
    }

    public void setScreenSize(Dimension dim){
	this.screenSizeHeight = dim.getHeight();
	this.screenSizeWidth = dim.getWidth();
	this.p_width = (this.p_width*(int) this.screenSizeWidth)/100;
	this.p_height = (this.p_height*(int) this.screenSizeHeight)/100;
	this.pos_x = (this.pos_x*(int) this.screenSizeWidth)/100;
	this.pos_y = (this.pos_y*(int) this.screenSizeHeight)/100;
	this.toGo_x = (this.toGo_x*(int) this.screenSizeWidth)/100;
	this.toGo_y = (this.toGo_y*(int) this.screenSizeHeight)/100;
	this.from_x = (this.from_x*(int) this.screenSizeWidth)/100;
	this.from_y = (this.from_y*(int) this.screenSizeHeight)/100;
    }

    public String getName(){
	return name;
    }


    public int getPosX(){
	return pos_x;
    }
    
    public int getPosY(){
	return pos_y;
    }

    public int getWidth(){
	return p_width;
    }

    public int getHeight(){
	return p_height;
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

	if(this.pos_x != this.toGo_x && this.time != 0 && this.timeUpdate*nbTimesUpdated < 1000*this.time){
	    this.pos_x = (int)(((float)(this.toGo_x - this.from_x) / (this.time*1000))*this.timeUpdate*(nbTimesUpdated+1)) + this.from_x;
	}

	if(this.pos_y != this.toGo_y && this.time != 0 && this.timeUpdate*nbTimesUpdated < 1000*this.time){
	    this.pos_y = (int)(((float)(this.toGo_y - this.from_y) / (this.time*1000))*this.timeUpdate*(nbTimesUpdated+1)) + this.from_y;
	}

	if(this.pos_x == this.toGo_x || this.time == 0 || this.timeUpdate*nbTimesUpdated >= 1000*this.time){
	    this.pos_x = toGo_x;
	    this.from_x = this.pos_x;
	}

	if(this.pos_y == this.toGo_y || this.time == 0 || this.timeUpdate*nbTimesUpdated >= 1000*this.time){
	    this.pos_y = toGo_y;
	    this.from_y = this.pos_y;
	}
	
	System.out.println(this);
	
	this.nbTimesUpdated++;
    }

    public void moveSin(){
	if(this.toGo_x != this.pos_x){
	    double teta = Math.atan((this.toGo_y - this.pos_y)/(this.toGo_x - this.pos_x)); 
	    System.out.println("angle: " + teta);  
	}
    }

    public void move(){
	fishLock.lock();
	this.moveStraight();
	fishLock.unlock();
    }

    
    public void updateFish(Fish newFish){
	fishLock.lock();
	if(this.toGo_x != newFish.getToGoX() || this.toGo_y != newFish.getToGoY()){
	    this.time = newFish.getTime();
	    this.toGo_x = (newFish.getToGoX()*(int) this.screenSizeWidth)/100;
	    this.toGo_y = (newFish.getToGoY()*(int) this.screenSizeHeight)/100;
	    this.nbTimesUpdated = 0;
	    this.from_x = this.pos_x;
	    this.from_y = this.pos_y;
	}
	fishLock.unlock();
    }

    @Override
    public void paintComponent(Graphics g) {

	if(this.pos_x < this.screenSizeWidth && this.pos_x > (0 - this.p_width) && this.pos_y > (0 - this.screenSizeHeight) && this.pos_y < this.screenSizeHeight){
	    fishLock.lock();
	    super.paintComponent(g);	  
	    
	    
	    this.setBounds(this.pos_x, this.pos_y, this.p_width, this.p_height);
	    g.drawImage(image, 0, 0, this.p_width, this.p_height, this);	    
	    fishLock.unlock();
	}
    }

    @Override
    public String toString(){
	return "nom: " + name + " pos: " + pos_x + ", " + pos_y + " taille: " + p_width + ", " + p_height;
    }
    
}
