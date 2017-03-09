import java.awt.*;
import javax.swing.*;
import java.awt.event.*;

public class BackgroundPanel extends JPanel {

    Image image;

    public BackgroundPanel(Image image) {
	this.image = image;
    }

    @Override
    public void paintComponent(Graphics g) {
	super.paintComponent(g);
	g.drawImage(image, 0, 0, this.getWidth(), this.getHeight(), null);
    }

}
