import javax.annotation.processing.SupportedSourceVersion;
import java.io.*;

import java.util.Random;


public class Main {

    public static void main(String[] args) throws IOException {
        String inputGraphFile = "/data1/zhuoxh/timestamp/sorted_deduplicated_youtube-u-growth.txt";    // graph file path
        

        double p = Double.parseDouble(args[0]);

        double global = 0;                        // global triangle count estimation
        
        double sample_edges = 0;                  // sampling edges of MASCOT

        long discoverd_triangles = 0;             // number of triangles that MASCOT detects
        
        Estimator estimator = new Estimator(p);
            
        double time0 = System.currentTimeMillis();
        run(estimator, inputGraphFile, "\t");
        double time1 = System.currentTimeMillis();
        double elpased_time = (time1 - time0) / 1000.0;  // running time of MASCOT
            
        estimator.output();
        estimator.computeLAPE();                     
            
        sample_edges = estimator.getSample_edges();

        global = estimator.getGlobalTriangle();
        discoverd_triangles = estimator.getDiscoverd_triangles();

            
        System.out.println("elpased_time:" + elpased_time + "s");
           
        System.out.println("sample_edges:" + estimator.getSample_edges());
        System.out.println("triangle detection:" + discoverd_triangles);
        System.out.println("global:" + String.format("%4f", estimator.getGlobalTriangle()));

    }

    
    private static void run(Estimator estimator, String inputGraphFile, String delim) throws IOException {
        BufferedReader br = new BufferedReader(new FileReader(inputGraphFile));

        long lineNum = 0;
        while(true) {
            String line = br.readLine();
            if(line == null) {
                break;
            }

            int[] edge = parseEdge(line, delim);

            estimator.processEdge(edge[0], edge[1]);

            if ((++lineNum) % 1000000000 == 0) {
                System.out.println("Number of edges processed: " + lineNum +", estimated number of global triangles: " + String.format("%4f", estimator.getGlobalTriangle()));
            }
        }
        System.out.println("MASCOT terminated ...");
        System.out.println("Estimated number of global triangles: " + String.format("%4f", estimator.getGlobalTriangle()));

        br.close();
    }

    private static int[] parseEdge(String line, String delim) {
        String[] tokens = line.split(delim);
        int src = Integer.valueOf(tokens[0]);
        int dst = Integer.valueOf(tokens[1]);

        return new int[]{src, dst};
    }
}