package ru.spb.osll.web.client.ui.core;

import ru.spb.osll.web.client.localization.Localizer;

import com.google.gwt.dom.client.Style.Unit;
import com.google.gwt.event.dom.client.ClickEvent;
import com.google.gwt.event.dom.client.ClickHandler;
import com.google.gwt.user.client.ui.Button;
import com.google.gwt.user.client.ui.DialogBox;
import com.google.gwt.user.client.ui.HTML;
import com.google.gwt.user.client.ui.HasHorizontalAlignment;
import com.google.gwt.user.client.ui.HorizontalPanel;
import com.google.gwt.user.client.ui.Label;
import com.google.gwt.user.client.ui.PasswordTextBox;
import com.google.gwt.user.client.ui.TextBox;
import com.google.gwt.user.client.ui.VerticalPanel;

public class UIUtil {
	
	public static TextBox getTextBox(int  visibleLength, int width){
		TextBox textBox = new TextBox();
		textBox.setVisibleLength(visibleLength);
		textBox.setWidth(width + "px");
		return textBox;
	}

	public static PasswordTextBox getPasswordTextBox(int visibleLength, int width){
		PasswordTextBox passwordTextBox = new PasswordTextBox();
		passwordTextBox.setVisibleLength(visibleLength);
		passwordTextBox.setWidth(width + "px");
		return passwordTextBox;
	}

	public static VerticalPanel getVerticalPanel(){
		return new VerticalPanel();
	}
	
	public static VerticalPanel getVerticalPanel(boolean alignCenter){
		VerticalPanel verticalPanel = new VerticalPanel();
		if (alignCenter){
			verticalPanel.setWidth("100%");
			verticalPanel.setHorizontalAlignment(HasHorizontalAlignment.ALIGN_CENTER);
		}
		return verticalPanel;
	}

	public static HorizontalPanel getHorizontalPanel(){
		return new HorizontalPanel();
	}
	
	public static HorizontalPanel getHorizontalPanel(boolean alignCenter){
		return getHorizontalPanel(alignCenter, 0);
	}

	public static HorizontalPanel getHorizontalPanel(int spacing){
		return getHorizontalPanel(false, spacing);
	}
	
	public static HorizontalPanel getHorizontalPanel(boolean alignCenter, int spacing){
		HorizontalPanel horizontalPanel = new HorizontalPanel();
		horizontalPanel.setSpacing(spacing);
		if (alignCenter){
			horizontalPanel.setWidth("100%");
			horizontalPanel.setHorizontalAlignment(HasHorizontalAlignment.ALIGN_CENTER);
		}
		return horizontalPanel;
	}
	
	public static DialogBox getSimpleDialog(String title, String message){
		return getSimpleDialog(title, message, Localizer.res().btnOk());
	}
	
	public static DialogBox getSimpleDialog(String title, String message, String btnText){
		final DialogBox dialogBox = new DialogBox();
		dialogBox.setText(title);
		dialogBox.setAnimationEnabled(true);
		dialogBox.setWidth("300px");

		final VerticalPanel dialogVPanel = getVerticalPanel(true);
		dialogVPanel.addStyleName("dialogVPanel");

		Button btn = new Button(btnText);
		btn.addClickHandler(new ClickHandler() {
			@Override
			public void onClick(ClickEvent event) {
				dialogBox.hide(true);
			}
		});
		
		dialogVPanel.add(new HTML(message));
		dialogVPanel.add(btn);
		dialogBox.setWidget(dialogVPanel);
		
		return dialogBox;
	}

	public static Label constructLabel(String title, int px, String width){
		Label label = constructLabel(title, px);
		label.setWidth(width);
		return label;
	}
	
	public static Label constructLabel(String title, int px){
		Label label = new Label(title);
		label.getElement().getStyle().setFontSize(px, Unit.PX);
		return label;
	}

	public static Label constructLabel(int px, String width){
		Label label = constructLabel(px);
		label.setWidth(width);
		return label;
	}

	public static Label constructLabel(int px){
		Label label = new Label();
		label.getElement().getStyle().setFontSize(px, Unit.PX);
		return label;
	}

}
