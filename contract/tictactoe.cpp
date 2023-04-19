#include<eosio/eosio.hpp>
#include<eosio/system.hpp>
#include <eosio/crypto.hpp>
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

	/*=======================
		creando la instancia:
	  =======================
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
	
	/*====================== nueva accion ==========================*/
	/* =============================================================*/
	ACTION create(const name &challenger, name &host){
		/* paso 1 de 2:
		.requerir host y challenger y asegurar que no sean el mismo jugador
		.usar EOSIO check para lograrlo
		*/
		check(host!=challenger, "no puede ser la misma persona");
		print("hello ", host);
		print("\nhello ", challenger);
		
	}

	/* paso 2 de 2:
	.crear una tabla de juegos y se guardara el nombre del host y el challenger (para evitar tener juegos viejos)
	.tampoco debe existir otro registro con esos nombres
	*/

	TABLE games_record{
		name host;
		name challenger;
		uint64_t timestamp;//cambiado
		uint64_t primary_key() const{ return host.value; }
		uint64_t by_secondary() const{ return challenger.value; }


		//esto era para serializar la estructura:
		EOSLIB_SERIALIZE(games_record, (host)(challenger)(timestamp))
	};

	/* configurar la tabla actual, incluido el nombre. Pasamos el nombre de la tabla y su estructura 
	a un constructor de EOSIO multi index, se puede hacer de muchas formas, aqui se hace con typedef
	El nombre debe ser compatible con EOSIO name, el cual debe ser de 12 chars o menos. 
	Asi es como se conocera la tabla en el blockchain. El segundo argumento despues de la estructura es el argumento.
	Y el nombre abstracto de nuestro tipo se llama srv index
	*/
	
	typedef multi_index<name("games"), games_record, indexed_by<name("timestamps"), const_mem_fun<games_record, uint64_t, &games_record::by_secondary>>> games_index;

	/*====	creando la instancia: ====*/
	//el nombre de usuario se guardara en nuestra tabla de indices multiples:
	
	ACTION askgame(name host, name challenger){
		require_auth(get_self());
		
		//chequeo si un guardado con el mismo host y challenger ya existe
		games_index games(get_self(), get_self().value);

		/*al realizar push action nuevamente, se genera un error, porque cleos nos da un indice repetido en la tabla de indices multiples, 
			por lo tanto hay que hacer que la funcion ask verifique si el juego ya fue incluido en la tabla. Primero hay que buscar el registro
			Ahora usamos find: devuelve un iterador*/

		auto idx = games.get_index<name("timestamps")>();
		for(auto itr = idx.begin(); itr != idx.end(); itr++) {
			eosio::print_f("Registro del acceso de usuario: ", itr->host, itr->timestamp);
			eosio::print_f("Registro del acceso del retador: ", itr->challenger, itr->timestamp);
		}

		//si el indice secundario tenia un nombre
		itr = idx.find("Ale"); //se puede usar idx.upper_bound(timestamp) o lower_bound para buscar los primeros o ultimos registros por timestamps



				
		//Create a new game record
		games.emplace(get_self(), [&](auto& new_game){
			new_game.host = host;
			new_game.challenger = challenger;
			new_game.timestamp = current_time_point();
		});
		
	}

	ACTION close(name host, name challenger){
		//verifico que juego existe en la tabla
		games_table games(get_self(), get_self().value);
		
		
		//verifico que esa accion se llamo por el host o challenger
		check(has_auth(host) || has_auth(challenger), "Solo el host o el challenger pueden cerrar el juego");

		//Uso erase para borrar el registro de la tabla games
		
	}
	
	


	// Agregamos la siguiente línea de código para vincular el archivo Ricardian a nuestro contrato:
    //static constexpr const char* const _ricardian_description = "tictactoe.ricardian";

};