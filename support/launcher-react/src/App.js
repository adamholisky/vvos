import logo from './logo.svg';
import './App.css';
import React, { useRef } from 'react';
import { VncScreen } from 'react-vnc';

function App() {
	const ref = useRef();

	return (
		<VncScreen
			url='ws://localhost:8000'
			scaleViewport
			background="#000000"
			style={{
				width: '1024px',
				height: '768px',
			}}
			ref={ref}
		/>	
	);
}

export default App;
