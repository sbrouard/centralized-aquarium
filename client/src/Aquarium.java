import java.awt.*;
import javax.swing.*;
import java.awt.event.*;
import java.io.*;
import javax.imageio.*;
import java.util.*;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.locks.ReentrantLock;

public class Aquarium extends JFrame{

    private ArrayList<Fish> fishes = new ArrayList<Fish>();
    private ReentrantLock fishesLock = new ReentrantLock();

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
	
	fishesLock.lock();
	
	for(Fish fish1 : f){
	    boolean toAdd = true;
	    
	    for(Iterator<Fish> it = this.fishes.iterator(); it.hasNext(); ){
		
		Fish fish2 = it.next();
		if(fish2.getName().equals(fish1.getName())){
		    fish2.updateFish(fish1);
		    toAdd = false;
		    break;
		}
	    }

	    if(toAdd){
		fish1.setScreenSize(this.getContentPane().getSize());
		this.fishes.add(fish1);
		this.add(fish1);
	    }
	}
	    
	for(Iterator<Fish> it = this.fishes.iterator(); it.hasNext(); ){
	    
	    boolean toDel = true;
	    Fish fish1 = it.next();

	    for(Fish fish2 : f){
		if(fish1.getName().equals(fish2.getName())){
		    toDel = false;
		    break;
		}
	    }
	    
	    if(toDel){
		this.remove(fish1);
		it.remove();
	    }
	}
	
	fishesLock.unlock();
    }
    
    public void update(){

	fishesLock.lock();

	for(Fish f : this.fishes){
	    f.move();
	    //System.out.println(f);
	}

	fishesLock.unlock();

	this.repaint();

	/* TO DEBUG
	System.out.println("update");
	System.out.println(this.fishes.toString());
	*/
    }
}
