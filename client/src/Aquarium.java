import java.awt.*;
import javax.swing.*;
import java.awt.event.*;
import java.io.*;
import javax.imageio.*;


public class Aquarium extends JFrame{

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

	//this.setUndecorated(true);
	//this.setExtendedState(JFrame.MAXIMIZED_BOTH);	

	this.setVisible(true);
    }

    public static void main(String[] args){
	new Aquarium();
    }
   
}
