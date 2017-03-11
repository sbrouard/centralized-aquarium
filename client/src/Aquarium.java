import java.awt.*;
import javax.swing.*;
import java.awt.event.*;
import java.io.*;
import javax.imageio.*;
import java.util.ArrayList;
import java.util.concurrent.TimeUnit;

public class Aquarium extends JFrame{

    private ArrayList<Fish> fishes = new ArrayList<Fish>();

    public Aquarium(){

	this.setTitle("Aquarium");
	this.setLocationRelativeTo(null);
	this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
	this.setLayout(new FlowLayout());

	try{
	    Image background = ImageIO.read(new File("aquarium.png"));
	    this.setContentPane(new BackgroundPanel(background));
	    
	} catch(IOException e){
	    System.err.println("Cannot read background image");
	}

	// Uncomment to make the cross disappear.
	/*
	this.setUndecorated(true);
	*/

	Fish magicarpe = new Fish("magicarpe", "magicarpe",-50 ,200 , 200, 200);
	this.add(magicarpe);

	
	this.setExtendedState(JFrame.MAXIMIZED_BOTH);	

	this.setVisible(true);
	
	try{
	    TimeUnit.SECONDS.sleep(2);
	} catch (Exception e){}
	
	magicarpe.setPosX(0);
	this.repaint();

    }

    public void setFishes(ArrayList<Fish> fishes){
    }
    
    public void update(){
	for(Fish f : fishes){
	    f.move();
	}
    }

    public static void main(String args[]){
	new Aquarium();
    }

}
