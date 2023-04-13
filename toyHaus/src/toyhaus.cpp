#include<eosio/eosio.hpp>
using namespace eosio;

CONTRACT toyhaus : public contract{
	public:
		using contract::contract;

	ACTION welcome(name arrival){
		print("welcome to the toy haus!");
	}
	private:
};