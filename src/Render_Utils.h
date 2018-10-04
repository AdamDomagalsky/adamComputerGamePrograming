#pragma once

#include "objload.h"

namespace Core
{
	void DrawVertexArray(const float * vertexArray, int numVertices, int elementSize);
	void DrawVertexArrayIndexed(const float * vertexArray, const int * indexArray, int numIndexes, int elementSize);


	struct VertexAttribute
	{
		const void * Pointer;
		int Size;
	};

	struct VertexData
	{
		static const int MAX_ATTRIBS = 8;
		VertexAttribute Attribs[MAX_ATTRIBS];
		int NumActiveAttribs;
		int NumVertices;
	};

	// Ta funkcja sluzy do rysowania trojkatow, ktorych wierzcholki moga byc opisane wiecej niz jednym atrybutem.
	// Funkcja przyjmuje jako parametr strukture, w ktorej nalezy zawrzec wszystkie potrzebne dane.
	// 
	// Przykladowe wywolanie funkcji - narysowanie trojkata jak na pierwszych zajeciach:
	/*
	
	const float vertices[] = {
		0.25f,  0.25f, 0.0f, 1.0f,
		0.25f, -0.25f, 0.0f, 1.0f,
		-0.25f, -0.25f, 0.0f, 1.0f
	};
	
	Core::VertexData vertexData;
	vertexData.NumActiveAttribs = 1;			// Liczba uzywanych atrybutow wierzcholka
	vertexData.Attribs[0].Pointer = vertices;	// Wskaznik na dane zerowego atrybutu
	vertexData.Attribs[0].Size = 4;				// Wielkosc zerowego atrybutu (ilosc liczb opisujacych ten atrybut w pojedynczym wierzcholku)
	vertexData.NumVertices = 3;					// Liczba wierzcholkow do narysowania
	Core::DrawVertexArray(vertexData);

	*/
	void DrawVertexArray(const VertexData & data);

	void DrawModel(obj::Model * model);
	void DrawModelNormal(obj::Model * model);
}