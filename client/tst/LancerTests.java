
import java.lang.reflect.Method;

class LancerTests {
    static private void lancer(Class c) throws Exception {
	Method m[] = c.getMethods();
	String t;

	Object a = c.newInstance();
	for(int i = 0; i < m.length; i++){
	    t = "" + m[i];
	    if(t.indexOf("test") >= 0){
		m[i].invoke(a);
		System.out.println(".");
	    }
	}
	System.out.println("OK");
    }

    static public void main(String[] args) throws Exception {
	boolean estMisAssertion = false;
	assert estMisAssertion = true;
	
	if (!estMisAssertion)
	    {
		System.out.println("Execution impossible sans l'option -ea");
		}

	for(int i = 0; i < args.length; i++){
	    System.out.println(args[i]);
	    Class c = Class.forName(args[i]);
	    lancer(c);
	}
	
	System.out.println("FINI");}			      



}
