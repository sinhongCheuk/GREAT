

import java.io.*;
import java.util.Map;
import java.util.Random;
import java.util.TreeMap;

/**
 * BatchIns Process
 */
public class BatchIns {

    /**
     * Example Code for WRS_INS
     * @throws IOException
     */
    public static void main(String[] args) throws IOException {
        double averageLAPE = 0;
        
        if(args.length < 4) {
            printError();
            System.exit(-1);
        }

        final String inputPath = args[0];
        System.out.println("input_path: " + inputPath);
        final String outputPath = args[1];
        System.out.println("output_path: " + outputPath);
        final int maxSampleNum = Integer.valueOf(args[2]);
        System.out.println("k: " + maxSampleNum);
        final double alpha = Double.valueOf(args[3]);
        System.out.println("alpha: " + alpha);

        WRSIns wrs = new WRSIns(maxSampleNum, alpha, new Random().nextInt());
      
        double time0 = System.currentTimeMillis();
        run(wrs, inputPath, "\t");
        double time1 = System.currentTimeMillis();
        double elpased_time = (time1 - time0) / 1000.0;
            
        wrs.output();
        averageLAPE += computeLAPE();

        System.out.println("elpased_time:" + elpased_time + "s");
        
        
        System.out.println("triangle detection:" + wrs.getDiscover_triangles() + "s");
        System.out.println("global triangle estimation:" + String.format("%4f", wrs.getGlobalTriangle()) + "s");
    }

    private static void printError() {
        System.err.println("Usage: run.sh graph_type input_path output_path k alpha");
        System.err.println("- k (maximum number of samples) should be an integer greater than or equal to 2.");
        System.err.println("- alpha (relative size of the waiting room) should be a real number in [0,1).");
    }

    private static void run(WRSIns wrs, String inputPath, String delim) throws IOException {
        BufferedReader br = new BufferedReader(new FileReader(inputPath));
        br.readLine();
        long count = 0;

        System.out.println("start running WRS_INS...");
        double read_time = 0;
        double triangle_time = 0;

        while(true) {
            final String line = br.readLine();
            if(line == null) {
                break;
            }
            double time0 = System.currentTimeMillis();
            int[] edge = parseEdge(line, delim);
            double time1 = System.currentTimeMillis();
            read_time += (time1 - time0) / 1000.0;

            double time2 = System.currentTimeMillis();
            wrs.processEdge(edge[0], edge[1]);
            double time3 = System.currentTimeMillis();
            triangle_time += (time3 - time2) / 1000.0;

            if((++count) % 100000000 == 0) {
                System.out.println("Number of edges processed: " + count +", estimated number of global triangles: " + String.format("%4f", wrs.getGlobalTriangle()));
            }
        }

        System.out.println("WRS_INS terminated ...");
        System.out.println("Estimated number of global triangles: " + String.format("%4f", wrs.getGlobalTriangle()));


        br.close();
    }

    private static int[] parseEdge(String line, String delim) {
        String[] tokens = line.split(delim);
        int src = Integer.valueOf(tokens[0]);
        int dst = Integer.valueOf(tokens[1]);

        return new int[]{src, dst};
    }


    /**
     * caculate local triangle estimation error
     */ 
    public static double computeLAPE() {
        
        String algorithmOutputFile = "/data1/local-wrs.txt";
        String groundTruthFile = "/data1/local-youtube-u-growth.txt";

        double averageLAPE = 0;

        try (
                BufferedReader groundTruthReader = new BufferedReader(new FileReader(groundTruthFile));
                BufferedReader algorithmOutputReader = new BufferedReader(new FileReader(algorithmOutputFile))
        ) {
            String groundTruthLine;  
            String algorithmOutputLine;  

            double totalLAPE = 0;
            int vertexCount = 0;

            while ((groundTruthLine = groundTruthReader.readLine()) != null &&
                    (algorithmOutputLine = algorithmOutputReader.readLine()) != null) {
                String[] groundTruthParts = groundTruthLine.split("\\s+");
                String[] algorithmOutputParts = algorithmOutputLine.split("\\s+");

                
                double groundTruthValue = Double.parseDouble(groundTruthParts[1]); // Converted to double
                double algorithmOutputValue = Double.parseDouble(algorithmOutputParts[1]);

                double lape = Math.abs(algorithmOutputValue - groundTruthValue) / (groundTruthValue + 1);
                totalLAPE += lape;

                vertexCount++;
            }
            System.out.println("vertex num in local file: " + vertexCount);

            averageLAPE = totalLAPE / vertexCount;
            System.out.println("Average Local Absolute Percentage Error (LAPE): " + averageLAPE);

        } catch (IOException e) {
            e.printStackTrace();
        }

        return averageLAPE;
    }
}
