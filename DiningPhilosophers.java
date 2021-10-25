import java.util.concurrent.locks.*; //Import for Locks

// This is the Monitor Class, that holds all the State Information for
// Dining Philosophers “Table”. 
// Only a Single Object of this class will be created and is, of
// course, shared among ALL Philosopher Threads, because it holds
// the entire State information for ALL Philosophers.
// So each Philosopher will need to hold a Reference to this Single
// DiningPhilosophers Object.
public class DiningPhilosophers
{
	// 5 Dining Philosophers
	private static final int N = 5;
	
	// Enumeration type for Philosopher's States
	public enum PHIL_STATES {THINKING, HUNGRY, EATING}
	
	//The state Array, holding the current state for each Philosopher
	private PHIL_STATES[] state;
	
	// The State Lock - which is effectively the Monitor Lock
	// This needs to be Acquired at the beginning of every Public
	// Method in this class And Released at the end of each Public Method.
	final Lock stateLock;
	
	// The Condition Array, holding the Condition Variable for Each Philosopher
	final Condition[] self;
	
	// Constructor - Creates the state Array and “self” Condition Variable
	// Array Each of these arrays is of size N, of course!
	// For example: self = new Condition[N];
	// Then creates a new ReentrantLock() assigned to stateLock.
	// Finally, it loops through every index 0..(N-1) and:
	// 	- initialise self[i] with a new condition Variable:
	// 	  self[i] = stateLock.newCondition();
	// 	- initialise state[i] to THINKING
	public DiningPhilosophers()
	{
		state = new PHIL_STATES[N];
		self = new Condition[N];
		stateLock = new ReentrantLock();
		
		for (int i = 0; i < N; i++)
		{
			state[i] = PHIL_STATES.THINKING;
			self[i] = stateLock.newCondition();
		}
	}
	
	// pickup Method - implemented as defined in Lecture Notes.
	// Note that Lock is Acquired at the Beginning of this Method and
	// Released at the end of it! Note that "await();" Throws an
	// InterruptedExceptionexception that needs to be caught in a try..catch
	// block. The Release of the Lock should, therefore, take place in
	// "finally" block after the catch - it will be executed in any case 
	// (exception or not!), so that Lock will be released anyway by this
	// Method (whether or not exception has occured!)
	public void pickup(int philID)
	{
		stateLock.lock();
		
		state[philID] = PHIL_STATES.HUNGRY;
		System.out.println("Phil " + philID + ": HUNGRY");
		
		test(philID);
		
		try
		{
			// Incase we still didn't eat
			while (state[philID] != PHIL_STATES.EATING)
			{
				self[philID].await();
			}
		}
		catch (InterruptedException ie)
		{
			System.out.println(ie);
		}
		finally
		{
			stateLock.unlock();
		}
	}
	
	// putdown Method - implemented as defined in Lecture Notes.
	// Note that Lock is Acquired at the Beginning of this Method and
	// Released at the end of it! No exception is thrown here, so no need for
	// try..catch block! However, need to release lock in ALL
	// normal/abnormal cases!!!!!!
	public void putdown(int philID)
	{
		stateLock.lock();
		
		state[philID] = PHIL_STATES.THINKING;
		System.out.println("Phil " + philID + ": THINKING");

		int left = (philID + N - 1)%N;
		int right = (philID + 1)%N;  

		test(left); 
		test(right);
		
		stateLock.unlock();
	}
	
	// test Method - implemented as defined in Lecture Notes.
	// Notice this is a private Method, that will be called only by
	// pickup/putdown Methods. Since both pickup and putdown are using
	// the Lock (acquired at the beginning and released at the end), then No
	// need to use the Lock here, because it is anyway invoked when the
	// Lock is acquired!
	private void test(int philID)
	{
		int left = (philID + N - 1)%N;
		int right = (philID + 1)%N;  
		
		if (state[philID] == PHIL_STATES.HUNGRY &&
			state[left] != PHIL_STATES.EATING &&
			state[right] != PHIL_STATES.EATING)
		{
			state[philID] = PHIL_STATES.EATING;
			System.out.println("Phil " + philID + ": EATING");
			self[philID].signal();
		}
	}
}