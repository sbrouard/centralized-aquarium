public class Fish{

    int pos_x;
    int pos_y;
    int width;
    int height;
    String name;
    String type;
    
    public Fish(String name, String type,int pos_x,int pos_y,int width,int height){
	this.name = name;
	this.type = type;
	this.pos_x = pos_x;
	this.pos_y = pos_y;
	this.width = width;
	this.height = height;
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
}
