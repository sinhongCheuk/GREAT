import it.unimi.dsi.fastutil.doubles.DoubleOpenHashSet;
import it.unimi.dsi.fastutil.ints.Int2BooleanOpenHashMap;
import it.unimi.dsi.fastutil.ints.Int2DoubleMap;
import it.unimi.dsi.fastutil.ints.Int2DoubleOpenHashMap;
import it.unimi.dsi.fastutil.ints.Int2ObjectOpenHashMap;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Random;

import java.io.*;


public class Estimator {


    private Int2ObjectOpenHashMap<Int2ObjectOpenHashMap<double[]>> neighbors = new Int2ObjectOpenHashMap<>();  // graph composed of the sampled edges
    
    private Int2DoubleOpenHashMap nodeToCount = new Int2DoubleOpenHashMap(); // local triangle counts
    private double globalTriangle = 0; // global triangles

    private int k; // size of the reservoir
    private double t = 0;  // number of streaming edges processed so far
    private int next_slot_index = 0;  // for top-k edges in reservoir sampling
    private int delete_index = 0;     // position of the choosen edges

    private int discoverd_triangles = 0;

    private int[][] reservoir;
    private final Random random = new Random();


    public Estimator(int sizeOfReservoir) {
        this.reservoir = new int[2][sizeOfReservoir];
        this.k = sizeOfReservoir;
    }


    /**
     * process the given edge
     * @param src source node of the given edge
     * @param dst destination node of the given edge
     */
    public void processEdge(int src, int dst) {
        if(src == dst) { //ignore self loop
            return;
        }

        t++;

        
        count(src, dst); //count triangles involved

        if (t < k + 1) {
            // top-k edges in reservoir sampling
            reservoir[0][next_slot_index] = src;
            reservoir[1][next_slot_index] = dst;
            next_slot_index++;

            sample(src, dst);
        } else {
            // now the sampling probability turns to be k / t

            double p = (double) k / t;
            double randomValue = random.nextDouble();
            if (randomValue < p) {
                // select a random edge in reservoir
                delete_index = random.nextInt(k);
                int srcToBeRemove = reservoir[0][delete_index];
                int dstToBeRemove = reservoir[1][delete_index];

                // delete the selected edge
                deleteEdge(srcToBeRemove, dstToBeRemove);

                // then sample the given edge
                reservoir[0][delete_index] = src;
                reservoir[1][delete_index] = dst;
                sample(src, dst);
               
            }
        }


    }
    
    /**
     * sample an edge to the subgraph
     * @param src source node of the given edge
     * @param dst destination node of the given edge
     */
    private void sample(int src, int dst) {

        if(!neighbors.containsKey(src)) {
            neighbors.put(src, new Int2ObjectOpenHashMap<>());
        }
        neighbors.get(src).put(dst, new double[] {});

        if(!neighbors.containsKey(dst)) {
            neighbors.put(dst, new Int2ObjectOpenHashMap<>());
        }
        neighbors.get(dst).put(src, new double[] {});
    }

    /**
     * delete an edge from the subgraph
     * @param src source node of the given edge
     * @param dst destination node of the given edge
     */
    private void deleteEdge(int src, int dst) {
        Int2ObjectOpenHashMap<double[]> map = neighbors.get(src);
        if (map != null) {
            map.remove(dst);
            if (map.isEmpty()) {
                neighbors.remove(src);
            }
        }

        map = neighbors.get(dst);
        if (map != null) {
            map.remove(src);
            if (map.isEmpty()) {
                neighbors.remove(dst);
            }
        }

    }

    
    /**
     * count triangles incident to the given edge
     * @param src source node of the given edge
     * @param dst destination node of the given edge
     */
    private void count(int src, int dst) {

        // if this edge has a new node, there cannot be any triangles
        if(!neighbors.containsKey(src) || !neighbors.containsKey(dst)) {
            return;
        }

        // source node to neighbors
        Int2ObjectOpenHashMap<double[]> srcMap = neighbors.get(src);

        // destination node to neighbors
        Int2ObjectOpenHashMap<double[]> dstMap = neighbors.get(dst);

        if(srcMap.size() > dstMap.size()) {
            Int2ObjectOpenHashMap<double[]> temp = srcMap;
            srcMap = dstMap;
            dstMap = temp;
        }

        // the sum of counts increased
        double countSum = 0;
        double weight;

        if (t <= k) {
            weight = 1;
        } else {
            weight = (k / t) * ((k - 1) / (t - 1));
        }

        // find common neighborhood
        for(int neighbor : srcMap.keySet()) {
            if (dstMap.containsKey(neighbor)) {
                discoverd_triangles++;

                double count = 1 / weight;

                countSum += count;
                nodeToCount.addTo(neighbor, count); // update the local triangle count of the common neighbor
            }
        }

        if(countSum > 0) {
            nodeToCount.addTo(src, countSum); // update the local triangle count of the source node
            nodeToCount.addTo(dst, countSum); // update the local triangle count of the destination node
            globalTriangle += countSum; // update the global triangle count
        }
    }



    public double getGlobalTriangle() {
        return globalTriangle;
    }

    public Int2DoubleMap getLocalTriangle() {
        return nodeToCount;
    }
    
    public int getDiscover_triangles() {
        return discoverd_triangles;
    }
    
    /**
     * output local triangle estimation to file
     */    
    public void output() throws IOException {
        String fileName = "/data1/zhuoxh/local-triest-i.txt";    // local triangle estimation file path

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
     * caculate local triangle estimation error
     */ 
    public void computeLAPE() {
        String algorithmOutputFile = "/data1/zhuoxh/local-triest-i.txt";      // local triangle estimation file path
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
