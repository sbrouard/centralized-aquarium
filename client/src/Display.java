
import java.net.*;
import java.io.*;
import java.util.concurrent.*;

public class Display implements Runnable{
    
    private Aquarium aquarium;
    private int timeUpdate;

    public Display(Aquarium aquarium, int timeUpdate){
	this.aquarium = aquarium;
	this.timeUpdate = timeUpdate;
    }

    public void run(){
	final ScheduledExecutorService executorService = Executors.newSingleThreadScheduledExecutor();
	executorService.scheduleAtFixedRate(new Runnable() {
		@Override
		public void run() {
		    aquarium.update();
		}
	    }, 0, timeUpdate, TimeUnit.MILLISECONDS);
    }
}
