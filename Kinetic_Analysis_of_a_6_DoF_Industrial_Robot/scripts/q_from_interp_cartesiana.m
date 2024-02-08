function q = q_from_interp_cartesiana(T_traj, R, q_ref)
%Calcula la evoluci�n de las variables articulares a partir de una sucesi�n
%de matrices de transformaci�n homog�neas correspondientes a una
%trayectoria obtenida en el espacio cartesiano
q = zeros(size(T_traj,3),6);
for i = 1:size(T_traj,3)
    q(i,:) = CinInv(T_traj(:,:,i), R, q_ref, 1);
    q_ref = q(i,:);
end
end

