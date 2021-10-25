public class DPManager
{
	//5 Philosophers in the System
	private static final int N = 5;
	
	//Philosophers Array
	private Philosopher[] philosophers;
	
	//The single ("passive" entity) table of DiningPhilosophers
	private DiningPhilosophers table;
	
	// The Constructor:
	// - Creates a new philosophers Array of size N
	// Creates a new DiningPhilosophers, assigned to table
	// Then loops through the philosophers array: create Each Philosopher
	// (it's ID will be the index of the philosopher in the array).
	// Then invoke the "start" Method for each Philosopher.
	public DPManager()
	{
		table = new DiningPhilosophers();
		philosophers = new Philosopher[N];
		
		for (int i = 0; i < N; i++)
		{
			philosophers[i] = new Philosopher(table, i);
		}
		
		for (int i = 0; i < N; i++)
		{
			philosophers[i].start();
		}
	}
	
	// Main method simply creates the DPManager Object, which then
	// creates the DiningPhilosophers table and all the philosophers and get
	// them to run!
	public static void main(String[] args)
	{
		DPManager manager = new DPManager();
	}
}