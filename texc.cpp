/*\section{The Wand directive}
This directive is needed to make the source file visible to Wand.
*/ 
#ifdef __WAND__
target[name[c2tex.exe]type[application]platform[;Windows]]
target[name[c2tex]type[application]]
#endif

/*\section{Include files}
\subsection{Standard C I/O}
Include this file to get all declarations for C standard I/O.
*/
#include <cstdio>

/*\subsection{Other standard C functions}
In this module \texttt{malloc} \texttt{realloc} and \texttt{free} is used. They are declared in \texttt{cstdlib}
*/
#include <cstdlib>

/*\section{Storing temporary strings}
Temporary strings is stored in objects of the \texttt{Buffer} class. The definition of \texttt{Buffer} looks like the following.
*/
class Buffer
	{
	public:
		Buffer();
		~Buffer();
		Buffer& clear();
		void write(FILE* sink);
		
		bool emptyIs()
			{
			/*Ths buffer is empty $\Leftrightarrow \mathrm{buffer_{end}}=\mathrm{buffer_{begin}}$*/
			return !(buffer_end-buffer_begin);
			}
			
		Buffer& add(unsigned char ch);
		Buffer& endTrimWhitespace();
		Buffer& beginTrimWhitespace();
		Buffer& beginTrimNewline();

	private:
		Buffer(const Buffer& b);
		void operator=(const Buffer& b);
	
		unsigned char* buffer_begin_begin;
		const unsigned char* buffer_begin;
		unsigned char* buffer_end;
		const unsigned char* buffer_end_end;
	};

/*\subsection{Creating a Buffer object}
A Buffer object can be created with the default constructor.
*/
Buffer::Buffer()
	{
	/*It is practical to allocate a buffer already in the constructor. By doing so, it is not nessecary for NULL-checking in the destructor.*/
	buffer_begin_begin=(unsigned char*)malloc(sizeof(char));
	buffer_begin=buffer_begin_begin;
	buffer_end_end=buffer_begin_begin+1;
	buffer_end=buffer_begin_begin;
	}

/*\subsection{Destruction}
The destructor frees the memory resources used by the Buffer object.
*/
Buffer::~Buffer()
	{free(buffer_begin_begin);}

/*\subsection{Clearing the buffer}
To clear the buffer, call \texttt{clear}
*/
Buffer& Buffer::clear()
	{
	/*By setting begin pointer equal to end pointer, the buffer is cleared.*/
	buffer_begin=buffer_begin_begin;
	buffer_end=buffer_begin_begin;
	return *this;
	}

/*\subsection{Adding character to buffer}
To add a characer to the end of the buffer, call \texttt{add}.
*/
Buffer& Buffer::add(unsigned char ch)
	{
	if(buffer_end==buffer_end_end)
		{
		unsigned int length_allocated=buffer_end_end - buffer_begin;
		unsigned int offset_begin=buffer_begin-buffer_begin_begin;
		unsigned int offset_end=buffer_end-buffer_begin_begin;
		
		length_allocated*=2;
		buffer_begin_begin=(unsigned char*)realloc(buffer_begin_begin,sizeof(char)*length_allocated);
		buffer_end=buffer_begin_begin+offset_end;
		buffer_end_end=buffer_begin_begin+length_allocated;
		buffer_begin=buffer_begin_begin+offset_begin;
		}
	*buffer_end=ch;
	++buffer_end;
	return *this;
	}

Buffer& Buffer::endTrimWhitespace()
	{
	if(buffer_end==buffer_begin)
		{return *this;}

	do
		{
		--buffer_end;
		if(*buffer_end > ' ')
			{break;}
		}
	while(buffer_end!=buffer_begin);
	++buffer_end;

	return *this;
	}

Buffer& Buffer::beginTrimWhitespace()
	{
	if(buffer_end==buffer_begin)
		{return *this;}

	do
	
		{
		if(*buffer_begin > ' ')
			{break;}
		++buffer_begin;
		}
	while(buffer_begin!=buffer_end);
	
	return *this;
	}

Buffer& Buffer::beginTrimNewline()
	{
	if(buffer_end==buffer_begin)
		{return *this;}

	do
		{
		if(*buffer_begin!='\n' && *buffer_begin!='\r')
			{break;}
		++buffer_begin;
		}
	while(buffer_begin!=buffer_end);
	
	return *this;
	}

void Buffer::write(FILE* file)
	{
	fwrite(buffer_begin,sizeof(char),buffer_end-buffer_begin,file);
	}



int main()
	{
	Buffer buff;
	int ch_in[2];
	FILE* output=stdout;
	FILE* input=stdin;

	while( (ch_in[0]=getc(input))!=EOF )
		{
		switch(ch_in[0])
			{
			case '/':
				{
				ch_in[1]=getc(input);

				if(ch_in[1]=='*')
					{
					if(!buff.emptyIs())
						{
						fputs("\n\\begin{lstlisting}\n",output);
						buff.endTrimWhitespace().beginTrimNewline().write(output);
						fputs("\n\\end{lstlisting}\n",output);
						buff.clear();
						}
					}
				else
					{buff.add(ch_in[0]).add(ch_in[1]);}
				}
				break;
			case '*':
				{
				ch_in[1]=getc(input);
				if(ch_in[1]=='/')
					{
					buff.write(output);
					buff.clear();
					}
				else
					{buff.add(ch_in[0]).add(ch_in[1]);}
				}
				break;
			default:
				buff.add(ch_in[0]);
			}
		}
	fputs("\\begin{lstlisting}\n",output);
	buff.endTrimWhitespace().beginTrimNewline().write(output);
	fputs("\\end{lstlisting}\n",output);
	buff.clear();
	return 0;
	}
