#include<eosio/eosio.hpp>
#include<eosio/system.hpp>
using namespace eosio;

CONTRACT tictactoe:public contract{

	public:
		using contract::contract;
		ACTION welcome(name host, name opponent){
			require_auth(get_self());
			print("welcome ",host);
			print("\nAnd ",opponent);
		}

	//guardando los nombres de usuario que interactuan con el contrato:
	TABLE surveillance_record{
		name subject;
		eosio::time_point timestamp;

		uint64_t primary_key() const{ return subject.value;	}

		//esto le indica como serializar a la estructura:
		EOSLIB_SERIALIZE(surveillance_record, (subject)(timestamp))

	};

	/* configurar la tabla actual, incluido el nombre. Pasamos el nombre de la tabla y su estructura 
		a un constructor de EOSIO multi index, se puede hacer de muchas formas, aqui se hace con typedef
		El nombre debe ser compatible con EOSIO name, el cual debe ser de 12 chars o menos. 
		Asi es como se conocera la tabla en el blockchain. El segundo argumento despues de la estructura es el argumento.
		Y el nombre abstracto de nuestro tipo se llama survillance index 
	*/
	
	typedef eosio::multi_index<name("surveillance"), surveillance_record>surveillance_index;

	/*====================
		creando la instancia:
	  ======================
	*/
	//el nombre de usuario se guardara en nuestra tabla de indices multiples:
	ACTION hi(name user){
		surveillance_index surveillance(get_self(), get_self().value);
		/*al realizar push action nuevamente, se genera un error, porque cleos nos da un indice repetido en la tabla de indices multiples, 
			por lo tanto hay que hacer que la funcion HI verifique si el usuario ya fue incluido en la tabla. Primero hay que buscar el registro
			Ahora usamos find: devuelve un iterador
		*/
		auto itr = surveillance.find(user.value);

		surveillance.emplace(get_self(), [&](auto& new_row){
			new_row.subject = user;
			new_row.timestamp = current_time_point(); //current_time_point necesita #include <eosio/system.hpp>
		});
	}
	


	// Agregamos la siguiente línea de código para vincular el archivo Ricardian a nuestro contrato:
    static constexpr const char* const _ricardian_description = "tictactoe.ricardian";

};