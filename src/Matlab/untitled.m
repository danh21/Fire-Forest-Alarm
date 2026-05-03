function varargout = untitled(varargin)
% UNTITLED MATLAB code for untitled.fig
%      UNTITLED, by itself, creates a new UNTITLED or raises the existing
%      singleton*.
%
%      H = UNTITLED returns the handle to a new UNTITLED or the handle to
%      the existing singleton*.
%
%      UNTITLED('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in UNTITLED.M with the given input arguments.
%
%      UNTITLED('Property','Value',...) creates a new UNTITLED or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before untitled_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to untitled_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help untitled

% Last Modified by GUIDE v2.5 15-Dec-2022 14:25:45

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @untitled_OpeningFcn, ...
                   'gui_OutputFcn',  @untitled_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT


% --- Executes just before untitled is made visible.
function untitled_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to untitled (see VARARGIN)

% Choose default command line output for untitled
handles.output = hObject;

% Update handles structure
guidata(hObject, handles);

global isFileChosen;
isFileChosen = 0;

% UIWAIT makes untitled wait for user response (see UIRESUME)
% uiwait(handles.figure1);


% --- Outputs from this function are returned to the command line.
function varargout = untitled_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;
global x;
x=serial('COM4','BAUD', 9600); % Make sure the baud rate and COM port is same as in Arduino IDE
fopen(x);


% --- Executes on button press in pushbutton1.
function pushbutton1_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global a isFileChosen;
[file,path] = uigetfile({'*.jpg*'; '*.png*'}, 'Select image file');
if ([file,path])
    isFileChosen = 1;
    a = imread([path,file]);
    axes(handles.axes1);
    imshow(a);
else
    if (~isFileChosen)
        msgbox('NO FILE CHOSEN');
    end
end



% --- Executes on button press in scanBtn.
function scanBtn_Callback(hObject, eventdata, handles)
% hObject    handle to scanBtn (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global isFileChosen a a2 x;
if (isFileChosen)
    r=a(:,:,1);
    g=a(:,:,2);
    b=a(:,:,3);
    a2 = rgb2ycbcr(a);
    axes(handles.axes2);
    imshow(a2);
    Y = a2(:,:,1);
    cb = a2(:,:,2);
    cr = a2(:,:,3);
    Ymean1 = mean(Y,2);
    cbmean1 = mean(cb,2);
    crmean1 = mean(cr,2);
    Ymean = mean(Ymean1);
    cbmean = mean(cbmean1);
    crmean = mean(crmean1);
    disp(Ymean);
    disp(cbmean);
    disp(crmean);

    out = r>g & g>b;                % rule 1
    axes(handles.axes3);
    imshow(out);

    out1 = r>190 & g>100 & b<140;   % rule 2
    axes(handles.axes5);
    imshow(out1);


    out2 =  Y>=cb & cr>=cb;         % rule 3 4
    % (cr-cb)>=70 | (cb-cr)>=70 ;
    % Y>=cb & cr>=cb ;
    axes(handles.axes4);
    imshow(out2);

    out3 = Y>=Ymean & cb<=cbmean & cr>=crmean;  % rule 5
    axes(handles.axes6);
    imshow(out3);

    out34 = abs(cr-cb)>=70 ;        % rule 6
    axes(handles.axes9);
    imshow(out34);

    out4 = cb<=120 & cr>=150;       % rule 7
    axes(handles.axes8);
    imshow(out4);

    out5 = out & out1 & out2 & out3 & out4 & out34 ;

    out55 = bwareaopen(out5,400);

    out6=imfill(out5,'holes');
    out7=bwmorph(out6,'dilate',3);
    out7=imfill(out7,'holes');

    out55 = bwareaopen(out5,230);

    fire = bwconncomp(out55);
    disp(fire);
    Fires = fire.NumObjects;

    if (Fires == 1)
      fprintf(x,1);
      a1=imoverlay(a,out7,'green');
      axes(handles.axes7);
      imshow(a1);
      xlabel('FIRE DETECTED: LEVEL 1')
    elseif (Fires >1 && Fires<= 5)
      fprintf(x,2);
      a1=imoverlay(a,out7,'green');
      axes(handles.axes7);
      imshow(a1);
      xlabel('FIRE DETECTED: LEVEL 2')
    elseif (Fires >5)
      fprintf(x,3);
      a1=imoverlay(a,out7,'green');
      axes(handles.axes7);
      imshow(a1);
      xlabel('FIRE DETECTED: LEVEL 3') 
    else
      fprintf(x,4);
      a1=imoverlay(a,out7,'green');
      axes(handles.axes7);
      imshow(a1);
      xlabel('NO FIRE DETECTED')
    end
else
    msgbox('NO FILE CHOSEN');
end
