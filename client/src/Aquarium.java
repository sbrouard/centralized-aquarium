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
	
	this.setExtendedState(JFrame.MAXIMIZED_BOTH);	

	this.setVisible(true);
	
	try{
	    TimeUnit.SECONDS.sleep(1);
	} catch (Exception e){}
	

    }


    public void setFishes(ArrayList<Fish> f){
	
	for(Fish fish1 : f){
	    
	    boolean toAdd = true;
	    
	    for(Fish fish2 : this.fishes){
		if(fish2.getName().equals(fish1.getName())){
		    fish2.updateFish(fish1);
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

	    if(toDel){
		this.remove(fish1);
		this.fishes.remove(fish1);
	    }
	}
    }
    
    public void update(){
	for(Fish f : this.fishes){
	    f.move();
	}

	this.repaint();

	/* TO DEBUG
	System.out.println("update");
	System.out.println(this.fishes.toString());
	*/
    }

    public static void main(String args[]){
	new Aquarium();
    }

}
