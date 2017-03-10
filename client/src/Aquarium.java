import java.awt.*;
import javax.swing.*;
import java.awt.event.*;
import java.io.*;
import javax.imageio.*;
import java.util.ArrayList;


public class Aquarium extends JFrame{

    private ArrayList<Fish> fishes = new ArrayList<Fish>();

    public Aquarium(){

	this.setTitle("Aquarium");
	this.setSize(400,400);
	this.setLocationRelativeTo(null);
	this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

	this.setLayout(new BorderLayout());

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

	this.add(new Fish("magicarpe", "magicarpe", 5, 5, 200, 200));

	this.setExtendedState(JFrame.MAXIMIZED_BOTH);	

	this.setVisible(true);
    }

    public void addFishes(ArrayList<Fish> fishes){
    }
    
    public void update(){
    }

    public static void main(String args[]){
	new Aquarium();
    }

}
