#pragma once
ref class RandomNumbers
{
	
protected:
	static RandomNumbers instance;
	
	/** 
  	* [MSB] Random number generator: A good random number source coupled to a discrete uniform sampler.
	*/
	MathNet::Numerics::Random::MersenneTwister^ randomNumberGenerator;
	
	RandomNumbers() 
	{
		randomNumberGenerator = gcnew MathNet::Numerics::Random::MersenneTwister();
	}

public:
	static RandomNumbers^ Instance() {
		return %instance;
	}
	
	// Returns a random number in [min,max]
	int Sample(int minIncluded, int maxIncluded) {
		return MathNet::Numerics::Distributions::DiscreteUniform::Sample(
			randomNumberGenerator,minIncluded,maxIncluded);
	}

	// Returns a random number in [0,maxExcluded)
	int Sample(int maxExcluded) {
		return MathNet::Numerics::Distributions::DiscreteUniform::Sample(
			randomNumberGenerator,0,maxExcluded-1);
	}

};

