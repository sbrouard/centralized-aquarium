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
	Fish magicarpe2 = new Fish("magicarpe2", "magicarpe", 50, 250, 300, 300);

	this.add(magicarpe2);
	this.add(magicarpe);

	
	this.setExtendedState(JFrame.MAXIMIZED_BOTH);	

	this.setVisible(true);
	
	try{
	    TimeUnit.SECONDS.sleep(1);
	} catch (Exception e){}
	
	magicarpe.setPosX(0);
	this.remove(magicarpe2);
	this.repaint();

    }

    public void setFishes(ArrayList<Fish> f){
	
	for(Fish fish1 : f){
	    
	    boolean toAdd = true;
	    
	    for(Fish fish2 : this.fishes){
		if(fish2.getName().equals(fish1.getName())){
		    toAdd = false;
		    break;
		}
	    }
	    
	    if(toAdd){
		this.fishes.add(fish1);
		this.add(fish1);
	    }
	}

	for(Fish fish1 : this.fishes){
	    
	    boolean toDel = true;

	    for(Fish fish2 : f){
		if(fish1.getName().equals(fish2.getName())){
		    toDel = false;
		    break;
		}
	    }

	    if(toDel)
		this.fishes.remove(fish1);
	}
    }
    
    public void update(){
	for(Fish f : this.fishes){
	    f.move();
	}
	this.repaint();
    }

    public static void main(String args[]){
	new Aquarium();
    }

}
