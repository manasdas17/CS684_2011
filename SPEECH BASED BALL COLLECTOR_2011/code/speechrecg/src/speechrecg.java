

import edu.cmu.sphinx.frontend.util.Microphone;
import edu.cmu.sphinx.jsapi.JSGFGrammar;
import edu.cmu.sphinx.recognizer.Recognizer;
import edu.cmu.sphinx.result.Result;
import edu.cmu.sphinx.util.props.ConfigurationManager;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.net.URL;
import javax.speech.recognition.GrammarException;
import javax.speech.recognition.RuleGrammar;
import javax.speech.recognition.RuleParse;


public class speechrecg 
{
   
    static Recognizer recog;
    static JSGFGrammar jg;
    static Microphone microphone;
    
  
    URL url=null;
    ConfigurationManager cm=null;
	
    public void execute() throws IOException, GrammarException  
    {
        System.out.print(" Loading recognizer ...");
        recog.allocate();
        System.out.println(" Ready");
       
        if (microphone.startRecording()) 
        {
            //: Set Up Name of Grammar 
            //for here mygram.gram
        	System.out.println("microphone working");
            loadAndRecognize("mygram");
        } 
        else 
        {
            System.out.println("Can't start the microphone");
        }

    
        recog.deallocate();
    }
    
    private void loadAndRecognize(String grammarName) throws IOException, GrammarException  
    {
        jg.loadJSGF(grammarName);
        
        dumpSampleSentences(grammarName);
        recognizeAndReport();
    }
    
    private void recognizeAndReport() throws GrammarException {
    	
        String mystr="";
        FileWriter fstream=null;
        BufferedWriter out=null;
        
        while (true)  
        {
        	System.out.println("speak");
        	
        	Result result = recog.recognize();
        	System.out.println("spoken");
            if(result==null)
                System.out.println("Result :NULL");
            else
                System.out.println("Result :NOT NULL & value is: "+result.toString());
           
            
            String bestResult = result.getBestFinalResultNoFiller();
           
            if(bestResult==null)
                System.out.println("bestResult :NULL");
            else
                System.out.println("bestResult :NOT NULL & value is: "+bestResult);
            
            mystr.concat(bestResult);
            RuleGrammar ruleGrammar = jg.getRuleGrammar();
            if(ruleGrammar==null)
                System.out.println("ruleGrammar :NULL");
            else
                System.out.println("ruleGrammar :NOT NULL");
            
            System.out.println("RuleGrammar-Name: "+ruleGrammar.getName());
            
            RuleParse ruleParse = ruleGrammar.parse(bestResult, null);
            System.out.println("RuleParse :"+ruleParse);
            
            if (ruleParse != null)
            {
                System.out.println("\nYou Said:  " + bestResult + "\n");
            	try{
            		fstream = new FileWriter("color.txt");
                	out = new BufferedWriter(fstream);
            		out.write(bestResult);
            		out.close();
            		fstream.close();
            		fstream = null;
            		out = null;
            	}catch(IOException ioException){
            		
            	}
            }
            else
                System.out.println("RULEPARSE IS NULL");
        }
    }
        
    private void dumpSampleSentences(String title) 
    {
        System.out.println(" ====== " + title + " ======");
        System.out.println("Speak one of: \n");
        jg.dumpRandomSentences(200);
        System.out.println(" ============================");
    }
    
    public void deallocate(String grammarName)
    {
        try 
        {
            //recog.deallocate();   
            jg.loadJSGF(grammarName);
        } 
        catch (IOException ex) 
        {
            System.out.println(ex);
            ex.printStackTrace();
        }
        //recog.deallocate();
    }
    
	public static void main(String[] args)
	{    	
		File f;
		try 
	    {
			URL url=new URL("file:///C:/speech/jsgf.config.xml");
	        ConfigurationManager cm = new ConfigurationManager(url);

	        recog = (Recognizer) cm.lookup("recognizer");
	            
	        jg = (JSGFGrammar) cm.lookup("jsgfGrammar");
	        microphone = (Microphone) cm.lookup("microphone");
	                     
	        speechrecg jsgf=new speechrecg();
	        jsgf.url=url;
	        jsgf.cm=cm;
	        try{
	        	f = new File("color.txt");
	        	f.createNewFile();
	        } 
	        catch(IOException ioException){
	        	
	        }
	        jsgf.execute();
	    }   
	    catch (Exception e) 
	    {
	    	System.out.println("Problem configuring recognizer" + e);
	    }
	}
}
