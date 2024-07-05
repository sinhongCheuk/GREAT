import it.unimi.dsi.fastutil.doubles.DoubleOpenHashSet;
import it.unimi.dsi.fastutil.ints.*;
import it.unimi.dsi.fastutil.ints.IntOpenHashSet;
import it.unimi.dsi.fastutil.ints.Int2BooleanOpenHashMap;
import java.util.*;
import java.io.*;

public class Estimator {
    private Int2ObjectOpenHashMap<IntOpenHashSet> neighbors = new Int2ObjectOpenHashMap<>();  // // graph composed of the sampled edges
    
    private Int2DoubleOpenHashMap nodeToCount = new Int2DoubleOpenHashMap(); // local triangle counts

    private double globalTriangle = 0;
    private double p;
    private double weight;    // the probability of a trianlge

    private int discoverd_triangles = 0;

    private double t = 0;     // number of streaming edges processed so far

    private final Random random = new Random();

    private int sample_edges = 0;


    public Estimator(double p) {
        this.p = p;
        this.weight = 1 / (p * p);
    }


    /**
     * process the given edge
     * @param src source node of the given edge
     * @param dst destination node of the given edge
     */
    public void processEdge(int src, int dst) {
        if (src == dst) { //ignore self loop
            return;
        }

        t++;

        
        count(src, dst); //count triangles involved
        

        sample(src, dst);
        
    }


    /**
     * sample an edge to the subgraph
     * @param src source node of the given edge
     * @param dst destination node of the given edge
     */
    private void sample(int src, int dst) {
        double randomValue = random.nextDouble();
        if (randomValue < p) {
            neighbors.computeIfAbsent(src, k -> new IntOpenHashSet()).add(dst);
            neighbors.computeIfAbsent(dst, k -> new IntOpenHashSet()).add(src);
            sample_edges++;
        }

    }

    /**
     * count triangles incident to the given edge
     * @param src source node of the given edge
     * @param dst destination node of the given edge
     */
    private void count(int src, int dst) {
    
        // there is no such a vertex in the subgraph
        if (!neighbors.containsKey(src) || !neighbors.containsKey(dst)) {
            return;
        }

        // Source node to neighbors
        IntOpenHashSet srcSet = neighbors.get(src);

        // Destination node to neighbors
        IntOpenHashSet dstSet = neighbors.get(dst);


        if (srcSet.size() > dstSet.size()) {
            IntOpenHashSet temp = srcSet;
            srcSet = dstSet;
            dstSet = temp;
        }

        double countSum = 0;

        // find common neighborhood
        for (int neighbor : srcSet) {
            if (dstSet.contains(neighbor)) {
                discoverd_triangles++;
                countSum += weight;
                nodeToCount.addTo(neighbor, weight);
            }
        }

        if(countSum > 0) {
            nodeToCount.addTo(src, countSum); // update the local triangle count of the source node
            nodeToCount.addTo(dst, countSum); // update the local triangle count of the destination node
            globalTriangle += countSum; // update the global triangle count
        }
    }

    public int getDiscoverd_triangles() {
        return discoverd_triangles;
    }

    public double getGlobalTriangle() {
        return globalTriangle;
    }

    public Int2DoubleMap getLocalTriangle() {
        return nodeToCount;
    }

    public int getSample_edges() {
        return sample_edges;
    }


    /**
     * output local triangle estimation to file
     */    
    public void output() throws IOException {
        String fileName = "/data1/zhuoxh/local-mascot.txt";                    // local triangle estimation file path

        BufferedWriter writer = new BufferedWriter(new FileWriter(fileName));
        for (int i = 0; i <= 3223589; i++) {
            if (i % 50000000 == 0) {
                System.out.println("writing node: " + i);
            }
            double count = nodeToCount.getOrDefault(i, 0.0); 
            writer.write(i + "\t" + count + "\n");
        }
        writer.close(); 
    }
    
    /**
     * caculate local triangle estimation error and print it
     */  
    public void computeLAPE() {
        String algorithmOutputFile = "/data1/zhuoxh/local-mascot.txt";        // local triangle estimation file path
        String groundTruthFile = "/data1/zhuoxh/local-youtube-u-growth.txt";  // local triangle groundtruth file path

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
            System.out.println("vertex num: " + vertexCount);

            double averageLAPE = totalLAPE / vertexCount;
            System.out.println("Average Local Absolute Percentage Error (LAPE): " + String.format("%4f", averageLAPE));

        } catch (IOException e) {
            e.printStackTrace();
        }
    
    }
}
